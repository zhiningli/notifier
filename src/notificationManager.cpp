#include "notificationManager.h"
#include <nlohmann/json.hpp>

// Singleton instance
NotificationManager& NotificationManager::getInstance() {
	static NotificationManager* instance = new NotificationManager();
	return *instance;
}

NotificationManager::NotificationManager() {
	// Initialize any necessary variables or resources here
	std::cout << "[INFO] NotificationManager initialized." << std::endl;
}

void NotificationManager::addSession(const std::string& sessionID) {
	std::unique_lock lock(managerMutex);

	if (sessionToNotificationMap.find(sessionID) != sessionToNotificationMap.end()) {
		std::cerr << "[INFO] SessionID: " << sessionID << " already exists in the map." << std::endl;
		return;
	}

	sessionToNotificationMap[sessionID] = std::set<std::string>();
	std::cout << "[INFO] SessionID: " << sessionID << " added successfully." << std::endl;
}


void NotificationManager::createNotification(const std::string& sessionID, const nlohmann::json& payload) {
	try {
		auto sessionIt = sessionToNotificationMap.find(sessionID);
		if (sessionIt == sessionToNotificationMap.end()) {
			std::cerr << "[ERROR] SessionID: " << sessionID << " not found. Unauthorized attempt!" << std::endl;
			return;
		}

		std::unique_lock lock(managerMutex);
		std::string title = payload["title"];
		std::string msg = payload["message"];

		std::optional<std::string> notificationIDOpt = allocateNotificationID();
		if (!notificationIDOpt.has_value()) {
			std::cerr << "[ERROR] No available notification IDs!" << std::endl;
			return;
		}
		std::string notificationID = notificationIDOpt.value();

		Notification notification = NotificationBuilder()
			.setTitle(title)
			.setMessage(msg)
			.setNotificationID(notificationID)  
			.setSessionID(sessionID)
			.setStatus(StatusEnum::Active)
			.setCreationTime(std::chrono::system_clock::now())
			.build();

		auto notificationPtr = std::make_shared<Notification>(notification);
		notifications.emplace(notificationID, notificationPtr);
		sessionToNotificationMap[sessionID].insert(notificationID);

		std::cout << "Notification \"" << notification.getTitle()
			<< "\" with session ID: " << sessionID << " added successfully." << std::endl;
		displayNotification(sessionID, notificationID);
	}
	catch (const std::exception& e) {
		std::cerr << "Exception in createNotification: " << e.what() << std::endl;
	}
}



void NotificationManager::updateNotification(const std::string& sessionID, const std::string& notificationID, const nlohmann::json& payload) {


	if (!isSessionAuthorized(sessionID, notificationID)) {
		return;
	}

	{
		std::unique_lock lock(managerMutex);

		auto it = notifications.find(notificationID);
		if (it != notifications.end()) {
			if (payload.contains("title")) it->second->setTitle(payload["title"]);
			if (payload.contains("message")) it->second->setMessage(payload["message"]);

			std::cout << "Notification ID: " << notificationID << " updated for Session: " << sessionID << std::endl;

		}
		else {
			std::cerr << "Notification ID: " << notificationID << " not found." << std::endl;
		}
	displayNotification(sessionID, notificationID);
	}
}


void NotificationManager::removeNotification(const std::string& sessionID, const std::string& notificationID) {
	if (!isSessionAuthorized(sessionID, notificationID)) {
		return;
	}

	std::unique_lock lock(managerMutex);

	auto sessionIt = sessionToNotificationMap.find(sessionID);
	if (sessionIt != sessionToNotificationMap.end()) {
		sessionIt->second.erase(notificationID);
		if (sessionIt->second.empty()) {
			sessionToNotificationMap.erase(sessionIt);  // Clean up if no notifications left
		}
	}

	notifications.erase(notificationID);
	freeNotificationID(notificationID);

	std::cout << "Notification ID: " << notificationID << " removed for Session: " << sessionID << std::endl;
}



void NotificationManager::displayNotification(const std::string& sessionID, const std::string& notificationID) {

	if (!isSessionAuthorized(sessionID, notificationID)) {
		return;
	}

	std::shared_lock lock(managerMutex);
	auto it = notifications.find(notificationID);
	if (it != notifications.end()) {
		auto notification = it->second;
		windowsAPI.showNotification(notification->getTitle(), notification->getMessage());
	}
	else {
		std::cerr << "No notification found with ID: " << notificationID << std::endl;
	}
}


void NotificationManager::displayAllNotifications(const std::string& sessionID) {
	std::shared_lock lock(managerMutex);

	auto sessionIt = sessionToNotificationMap.find(sessionID);
	if (sessionIt == sessionToNotificationMap.end()) {
		std::cerr << "[ERROR] SessionID: " << sessionID << " not found. Unauthorized attempt!" << std::endl;
		return;
	}

	for (const auto& notificationID : sessionIt->second) {
		auto it = notifications.find(notificationID);
		if (it != notifications.end()) {
			windowsAPI.showNotification(it->second->getTitle(), it->second->getMessage());
		}
	}
}



NotificationManager::~NotificationManager() {
	std::cout << "[INFO] Destroying NotificationManager. Total active notifications: "
		<< notifications.size() << std::endl;

	std::unique_lock lock(managerMutex);
	notifications.clear();
	sessionToNotificationMap.clear();

	std::cout << "[INFO] Notification Manager destroyed." << std::endl;
}



std::optional<std::string> NotificationManager::allocateNotificationID() {
	std::unique_lock lock(idMutex);

	for (size_t i = 0; i < usedNotificationIDs.size(); ++i) {
		if (!usedNotificationIDs.test(i)) {
			usedNotificationIDs.set(i);
			return std::to_string(i);
		}
	}
	return std::nullopt; 
}



void NotificationManager::freeNotificationID(const std::string& notificationID) {
	std::unique_lock lock(idMutex);

	try {
		int id = std::stoi(notificationID);
		if (id >= 0 && id < 256) {
			usedNotificationIDs.reset(id);
		}
		else {
			std::cerr << "Invalid notification ID: " << notificationID << std::endl;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error in freeNotificationID: " << e.what() << std::endl;
	}
}

bool NotificationManager::isSessionAuthorized(const std::string& sessionID, const std::string& notificationID) {
	std::shared_lock lock(managerMutex);

	auto sessionIt = sessionToNotificationMap.find(sessionID);
	if (sessionIt == sessionToNotificationMap.end()) {
		std::cerr << "[ERROR] SessionID: " << sessionID << " not found. Unauthorized attempt!" << std::endl;
		return false;
	}

	if (sessionIt->second.find(notificationID) == sessionIt->second.end()) {
		std::cerr << "[ERROR] Unauthorized attempt by Session: " << sessionID
			<< " to access Notification: " << notificationID << std::endl;
		return false;
	}

	return true;
}