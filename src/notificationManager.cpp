#include "notificationManager.h"
#include "terminalUI.h"
#include "windows_api.h"
#include <nlohmann/json.hpp>


NotificationManager& NotificationManager::getInstance() {
    static NotificationManager instance;
    return instance;
}

NotificationManager::NotificationManager() {}

void NotificationManager::addSession(const std::string& sessionID) {
    if (sessionToNotificationMap.count(sessionID)) {
        std::cerr << "[INFO] SessionID: " << sessionID << " already exists in the map." << std::endl;
        return;
    }

    sessionToNotificationMap[sessionID] = {};
    TerminalUI::refreshScreen();
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

    auto notification = std::make_unique<Notification>(
        NotificationBuilder()
        .setTitle(title)
        .setMessage(msg)
        .setNotificationID(notificationID)
        .setSessionID(sessionID)
        .setStatus(StatusEnum::Active)
        .setCreationTime(std::chrono::system_clock::now())
        .build()
    );

    notifications[notificationID] = std::move(notification);
    sessionToNotificationMap[sessionID].insert(notificationID);

    TerminalUI::refreshScreen();
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
        TerminalUI::refreshScreen();
        displayNotification(sessionID, notificationID);
    }
    else {
        std::cerr << "Notification ID: " << notificationID << " not found." << std::endl;
    }

}

void NotificationManager::removeNotification(const std::string& sessionID, const std::string& notificationID) {
    if (!isSessionAuthorized(sessionID, notificationID)) {
        return;
    }

    sessionToNotificationMap[sessionID].erase(notificationID);

    notifications.erase(notificationID); 
    freeNotificationID(notificationID);
    TerminalUI::refreshScreen();
}


void NotificationManager::removeSession(const std::string& sessionID) {
    auto sessionIt = sessionToNotificationMap.find(sessionID);
    if (sessionIt == sessionToNotificationMap.end()) {
        std::cerr << "[ERROR] SessionID: " << sessionID << " not found. Skipping removal.\n";
        return;
    }

    for (const auto& notificationID : sessionIt->second) {
        notifications.erase(notificationID);
        freeNotificationID(notificationID);
    }

    sessionToNotificationMap.erase(sessionID);

    TerminalUI::refreshScreen();
}



void NotificationManager::displayNotification(const std::string& sessionID, const std::string& notificationID) {
    if (!isSessionAuthorized(sessionID, notificationID)) {
        return;
    }

    auto it = notifications.find(notificationID);
    if (it != notifications.end()) {
        WindowsAPI::showNotification(it->second->getTitle(), it->second->getMessage());
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
            WindowsAPI::showNotification(it->second->getTitle(), it->second->getMessage());
        }
    }
}

std::unordered_map<std::string, std::pair<std::string, std::string>> NotificationManager::getActiveNotifications() {
    std::unordered_map<std::string, std::pair<std::string, std::string>> activeNotifications;

    for (const auto& [notificationID, notification] : notifications) {
        activeNotifications[notificationID] = { notification->getTitle(), notification->getMessage() };
    }

    return activeNotifications;
}

std::set<std::string> NotificationManager::getActiveSessions() {
    std::set<std::string> activeSessions;
    for (const auto& [sessionID, _] : sessionToNotificationMap) {
        activeSessions.insert(sessionID);
    }
    return activeSessions;
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
