#include "notificationManager.h"

// Singleton instance
NotificationManager& NotificationManager::getInstance() {
	static NotificationManager* instance = new NotificationManager();
	return *instance;
}


NotificationManager::NotificationManager() : stopExpiryChecker(false) {
	std::cout << "NotificationManager Constructor Called" << std::endl;
	//expiryCheckerThread = std::thread(&NotificationManager::expiryChecker, this);
}

void NotificationManager::addNotification(const Notification& notification) {
	try {
		std::cout << "NotificationManager address: " << this << std::endl;
		std::cout << "managerMutex address: " << &managerMutex << std::endl;

		//std::lock_guard<std::mutex> lock(managerMutex);
		std::cout << "Adding notification: " << notification.getTitle() << std::endl;

		auto sessionID = notification.getSessionID();

		// Create a shared_ptr for the notification
		auto notificationPtr = std::make_shared<Notification>(notification);

		// Insert the notification into the map
		auto [it, inserted] = notifications.emplace(sessionID, notificationPtr);

		if (!inserted) {
			std::cout << "Notification with session ID: " << sessionID << " already exists." << std::endl;
			return;
		}

		// Add the shared_ptr to the expiry set
		expirySet.insert(it->second);

		std::cout << "Notification \"" << notification.getTitle()
			<< "\" with session ID: " << sessionID << " added successfully." << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in addNotification: " << e.what() << std::endl;
	}
}



void NotificationManager::removeNotification(const std::string& sessionID) {
	std::lock_guard<std::mutex> lock(managerMutex);

	auto it = notifications.find(sessionID);
	if (it != notifications.end()) {
		auto notification = it->second;

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
	//std::lock_guard<std::mutex> lock(managerMutex);

	for (const auto& [sessionID, notification] : notifications) {
		windowsAPI.showNotification(notification->getTitle(), notification->getMessage());
	}
}



void NotificationManager::displayLatestNotification() {
	std::cout << "Entering display latest notification function" << std::endl;
	//std::lock_guard<std::mutex> lock(managerMutex);
	std::cout << "Successfuly lock resources" << std::endl;
	if (!expirySet.empty()) {
		auto latest = *expirySet.begin();  // Get the first notification
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
		auto expired = *itr;

		std::cout << "Removing expired notification: " << expired->getTitle() << std::endl;

		// Remove from both containers
		notifications.erase(expired->getSessionID());
		itr = expirySet.erase(itr);
	}
}


void NotificationManager::updateNotificationExpiry(const std::string& sessionID,
	const std::chrono::system_clock::time_point& newExpiry) {
	std::lock_guard<std::mutex> lock(managerMutex);

	auto it = notifications.find(sessionID);
	if (it != notifications.end()) {
		auto notification = it->second;

		// Remove old instance from expiry set
		expirySet.erase(notification);

		// Update expiry time
		notification->setExpiryTime(newExpiry);

		// Reinsert into expiry set
		expirySet.insert(notification);

		std::cout << "Updated expiry for notification: " << notification->getTitle()
			<< " to new expiry time." << std::endl;
	}
	else {
		std::cerr << "No notification found with session ID: " << sessionID << std::endl;
	}
}



NotificationManager::~NotificationManager() {
	std::cout << "NotificationManager Destructor Called" << std::endl;
	stopExpiryChecker.store(true);

	if (expiryCheckerThread.joinable()) {
		expiryCheckerThread.join();
	}

	std::lock_guard<std::mutex> lock(managerMutex);
	notifications.clear();
	expirySet.clear();
	std::cout << "Notification Manager destroyed" << std::endl;
}



void NotificationManager::expiryChecker() {
	while (!stopExpiryChecker.load()) {
		std::this_thread::sleep_for(std::chrono::seconds(10));

		std::lock_guard<std::mutex> lock(managerMutex);

		auto now = std::chrono::system_clock::now();

		while (!expirySet.empty()) {
			auto earliest = *expirySet.begin();

			if (earliest->getExpiryTime() <= now) {
				std::cout << "Notification expired: " << earliest->getTitle() << std::endl;
				notifications.erase(earliest->getSessionID());
				expirySet.erase(expirySet.begin());
			}
			else {
				break;  // Earliest notification is not expired
			}
		}
	}

	std::cout << "Expiry checker stopped." << std::endl;
}



