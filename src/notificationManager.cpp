#include "notificationManager.h"
#include <nlohmann/json.hpp>

// Singleton instance
NotificationManager& NotificationManager::getInstance() {
    static NotificationManager instance;
    return instance;
}

NotificationManager::NotificationManager() {
    std::cout << "[INFO] NotificationManager initialized." << std::endl;
}

void NotificationManager::addSession(const std::string& sessionID) {
    if (sessionToNotificationMap.count(sessionID)) {
        std::cerr << "[INFO] SessionID: " << sessionID << " already exists in the map." << std::endl;
        return;
    }

    sessionToNotificationMap[sessionID] = {};
    std::cout << "[INFO] SessionID: " << sessionID << " added successfully." << std::endl;
}

void NotificationManager::createNotification(const std::string& sessionID, const nlohmann::json& payload) {
    if (!sessionToNotificationMap.count(sessionID)) {
        std::cerr << "[ERROR] SessionID: " << sessionID << " not found. Unauthorized attempt!" << std::endl;
        return;
    }

    std::string title = payload["title"];
    std::string msg = payload["message"];

    std::optional<std::string> notificationIDOpt = allocateNotificationID();
    if (!notificationIDOpt) {
        std::cerr << "[ERROR] No available notification IDs!" << std::endl;
        return;
    }
    std::string notificationID = *notificationIDOpt;

    auto notification = std::make_shared<Notification>(
        NotificationBuilder()
        .setTitle(title)
        .setMessage(msg)
        .setNotificationID(notificationID)
        .setSessionID(sessionID)
        .setStatus(StatusEnum::Active)
        .setCreationTime(std::chrono::system_clock::now())
        .build()
    );

    notifications[notificationID] = notification;
    sessionToNotificationMap[sessionID].insert(notificationID);

    std::cout << "Notification \"" << notification->getTitle()
        << "\" with session ID: " << sessionID << " added successfully." << std::endl;

    displayNotification(sessionID, notificationID);
}

void NotificationManager::updateNotification(const std::string& sessionID, const std::string& notificationID, const nlohmann::json& payload) {
    if (!isSessionAuthorized(sessionID, notificationID)) {
        return;
    }

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

void NotificationManager::removeNotification(const std::string& sessionID, const std::string& notificationID) {
    if (!isSessionAuthorized(sessionID, notificationID)) {
        return;
    }

    sessionToNotificationMap[sessionID].erase(notificationID);
    if (sessionToNotificationMap[sessionID].empty()) {
        sessionToNotificationMap.erase(sessionID);
    }

    notifications.erase(notificationID);
    freeNotificationID(notificationID);

    std::cout << "Notification ID: " << notificationID << " removed for Session: " << sessionID << std::endl;
}

void NotificationManager::displayNotification(const std::string& sessionID, const std::string& notificationID) {
    if (!isSessionAuthorized(sessionID, notificationID)) {
        return;
    }

    auto it = notifications.find(notificationID);
    if (it != notifications.end()) {
        windowsAPI.showNotification(it->second->getTitle(), it->second->getMessage());
    }
    else {
        std::cerr << "No notification found with ID: " << notificationID << std::endl;
    }
}

void NotificationManager::displayAllNotifications(const std::string& sessionID) {
    if (!sessionToNotificationMap.count(sessionID)) {
        std::cerr << "[ERROR] SessionID: " << sessionID << " not found. Unauthorized attempt!" << std::endl;
        return;
    }

    for (const auto& notificationID : sessionToNotificationMap[sessionID]) {
        auto it = notifications.find(notificationID);
        if (it != notifications.end()) {
            windowsAPI.showNotification(it->second->getTitle(), it->second->getMessage());
        }
    }
}

NotificationManager::~NotificationManager() {
    std::cout << "[INFO] Destroying NotificationManager. Total active notifications: "
        << notifications.size() << std::endl;

    notifications.clear();
    sessionToNotificationMap.clear();

    std::cout << "[INFO] NotificationManager destroyed." << std::endl;
}

std::optional<std::string> NotificationManager::allocateNotificationID() {
    for (size_t i = 0; i < usedNotificationIDs.size(); ++i) {
        if (!usedNotificationIDs.test(i)) {
            usedNotificationIDs.set(i);
            return std::to_string(i);
        }
    }
    return std::nullopt;
}

void NotificationManager::freeNotificationID(const std::string& notificationID) {
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
    return sessionToNotificationMap.count(sessionID) &&
        sessionToNotificationMap[sessionID].count(notificationID);
}
