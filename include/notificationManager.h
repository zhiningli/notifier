#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include "notification.h"
#include "windows_api.h"
#include <string>
#include <unordered_map>
#include <set>
#include <iostream>
#include <shared_mutex>
#include <thread>
#include <atomic>
#include <bitset>
#include <fstream>
#include <nlohmann/json.hpp>

class NotificationManager {
public:
    static NotificationManager& getInstance();

    void addSession(const std::string& sessionID);

    void createNotification(const std::string& sessionID, const nlohmann::json& payload);
    void updateNotification(const std::string& sessionID, const std::string& notificationID, const nlohmann::json& payload);
    void removeNotification(const std::string& sessionID, const std::string& notificationID);
    void displayNotification(const std::string& sessionID, const std::string& notificationID);
    void displayAllNotifications(const std::string& sessionID);

    ~NotificationManager();

private:
    std::unordered_map<std::string, std::shared_ptr<Notification>> notifications;
    std::unordered_map<std::string, std::set<std::string>> sessionToNotificationMap;
    std::bitset<256> usedNotificationIDs;
    std::shared_mutex managerMutex;
    std::shared_mutex idMutex;

    std::optional<std::string> allocateNotificationID();
    void freeNotificationID(const std::string& notificationID);
    bool isSessionAuthorized(const std::string& sessionID, const std::string& notificationID);

    WindowsAPI windowsAPI;

    NotificationManager();
    NotificationManager(const NotificationManager&) = delete;
    NotificationManager& operator=(const NotificationManager&) = delete;
};

#endif  // NOTIFICATION_MANAGER_H
