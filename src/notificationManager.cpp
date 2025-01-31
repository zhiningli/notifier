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

void NotificationManager::removeExpiredNotifications() {
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
	std::lock_guard<std::mutex> lock(managerMutex);
	notifications.clear();
	expirySet.clear();
	std::cout << "Notification Manager destroyed" << std::endl;
}


