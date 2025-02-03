#include "notificationManager.h"

// Singleton instance
NotificationManager& NotificationManager::getInstance() {
	static NotificationManager* instance = new NotificationManager();
	return *instance;
}


NotificationManager::NotificationManager() : stopExpiryChecker(false) {
	std::cout << "NotificationManager Constructor Called" << std::endl;
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



NotificationManager::~NotificationManager() {
	std::cout << "NotificationManager Destructor Called" << std::endl;
	stopExpiryChecker.store(true);

	if (expiryCheckerThread.joinable()) {
		expiryCheckerThread.join();
	}

	std::lock_guard<std::mutex> lock(managerMutex);
	notifications.clear();
	std::cout << "Notification Manager destroyed" << std::endl;
}
