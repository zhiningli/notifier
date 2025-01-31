#include "notificationManager.h"

// Singleton instance
NotificationManager& NotificationManager::getInstance() {
	static NotificationManager instance;
	return instance;
}

void NotificationManager::addNotification(const Notification& notification) {
	std::lock_guard<std::mutex> lock(managerMutex);

	auto sessionID = notification.getSessionID();
	notifications[sessionID] = notification;
	if (notifications.find(sessionID) == notifications.end()) {
		std::cout << "Notification with session ID : " << sessionID << " already exists" << std::endl;
		return;
	}

	// Add to expiry set
	expirySet.insert(&notifications[sessionID]);

	std::cout << "Added notification "<< notification.getTitle() << "with session ID : " << notification.getSessionID() << std::endl;
}

void NotificationManager::removeNotification(const std::string& sessionID) {
	std::lock_guard<std::mutex> lock(managerMutex);

	auto it = notifications.find(sessionID);
	if (it != notifications.end()) {
		auto notification = &it->second;

		// Remove from expiry set
		auto range = expirySet.equal_range(notification);
		for (auto itr = range.first; itr != range.second; ++itr) {
			if (*itr == notification) {
				expirySet.erase(itr);
				break;
			}
		}

		// Remove from main map
		notifications.erase(it);
		std::cout << "Removed notification with session ID: " << sessionID << std::endl;
	}
	else {
		std::cerr << "No notification found with session ID: " << sessionID << std::endl;
	}
}




void NotificationManager::displayAllNotifications() {
	std::lock_guard<std::mutex> lock(managerMutex);
	for (const auto& [sessionID, notification] : notifications){
		WindowsAPI::showNotification(notification.getTitle(), notification.getMessage());
		}
}


void NotificationManager::displayLatestNotification() {
	std::lock_guard<std::mutex> lock(managerMutex);

	if (!expirySet.empty()) {
		const Notification* latest = *expirySet.begin(); 
		windowsAPI.showNotification(latest->getTitle(), latest->getMessage());
	}
	else {
		std::cout << "No notifications available to display." << std::endl;
	}
}

void NotificationManager::updateExpiredNotifications() {
	std::lock_guard<std::mutex> lock(managerMutex);

	auto now = std::chrono::system_clock::now();
	auto itr = expirySet.begin();

	while (itr != expirySet.end() && (*itr)->isExpired()) {
		const Notification* expired = *itr;
		std::cout << "Removing expired notification: " << expired->getTitle() << std::endl;

		notifications.erase(expired->getSessionID());
		itr = expirySet.erase(itr); // Remove from multiset
	}
}

void NotificationManager::updateNotificationExpiry(const std::string& sessionID, const std::chrono::system_clock::time_point& newExpiry) {
	std::lock_guard<std::mutex> lock(managerMutex);

	auto it = notifications.find(sessionID);
	if (it != notifications.end()) {
		Notification* notification = &it->second;

		// Remove old instance from expiry set
		auto range = expirySet.equal_range(notification);
		for (auto itr = range.first; itr != range.second; ++itr) {
			if (*itr == notification) {
				expirySet.erase(itr);
				break;
			}
		}

		// Update expiry time and reinsert
		notification->setExpiryTime(newExpiry);
		expirySet.insert(notification);

		std::cout << "Updated expiry for notification: " << notification->getTitle() << " to new expiry time." << std::endl;
	}
	else {
		std::cerr << "No notification found with session ID: " << sessionID << std::endl;
	}
}


NotificationManager::~NotificationManager() {
	stopExpiryChecker = true; // Signal expiry checker thread to stop
	if (expiryCheckerThread.joinable()) {
		expiryCheckerThread.join(); // Wait for expiry checker thread to finish
	}
	std::lock_guard<std::mutex> lock(managerMutex);
	notifications.clear();
	expirySet.clear();
	std::cout << "Notification Manager destroyed" << std::endl;
}



NotificationManager::NotificationManager() : stopExpiryChecker(false) {
	expiryCheckerThread = std::thread(&NotificationManager::expiryChecker, this);
}


void NotificationManager::expiryChecker() {
	while (!stopExpiryChecker.load()) {
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

		{
			std::lock_guard<std::mutex> lock(managerMutex);

			if (!expirySet.empty()) {
				const Notification* earlieastNotification = *expirySet.begin();
				auto earliestExpiry = earlieastNotification->getExpiryTime();

				if (earliestExpiry <= now) {
					updateExpiredNotifications();
				}
				else {
					auto durationUntilNextExpiry = std::chrono::duration_cast<std::chrono::seconds>(earliestExpiry - now);
					auto sleepDuraction = std::min(durationUntilNextExpiry, std::chrono::seconds(300));
					std::this_thread::sleep_for(sleepDuraction);
				}
			}
			else {
				std::this_thread::sleep_for(std::chrono::seconds(300));
			}

			// Check for stop signal
			if (stopExpiryChecker.load()) {
				break;
			}

			// Short sleep to allow thread interruption in edge cases
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			std::cout << "Expiry checker thread stopped." << std::endl;
		}
	}
}


