#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include "notification.h"
#include <string>
#include <unordered_map>
#include <set>
#include <bitset>
#include <optional>
#include <nlohmann/json.hpp>

class NotificationManager {
public:
    static NotificationManager& getInstance();

    void addSession(const std::string& sessionID);
    std::string createNotification(const std::string& sessionID, const nlohmann::json& payload);
    void updateNotification(const std::string& sessionID, const std::string& notificationID, const nlohmann::json& payload);
    void removeNotification(const std::string& sessionID, const std::string& notificationID);
    void removeSession(const std::string& sessionID);
    void displayNotification(const std::string& sessionID, const std::string& notificationID);
    void displayAllNotifications(const std::string& sessionID);

    std::set<std::string> getActiveSessions(); 
    std::unordered_map<std::string, std::pair<std::string, std::string>> getActiveNotifications(); 

    ~NotificationManager();

private:
    std::unordered_map<std::string, std::unique_ptr<Notification>> notifications;
    std::unordered_map<std::string, std::set<std::string>> sessionToNotificationMap;
    std::bitset<256> usedNotificationIDs;

    std::optional<std::string> allocateNotificationID();
    void freeNotificationID(const std::string& notificationID);
    bool isSessionAuthorized(const std::string& sessionID, const std::string& notificationID);

    NotificationManager();
    NotificationManager(const NotificationManager&) = delete;
    NotificationManager& operator=(const NotificationManager&) = delete;
};

#endif  // NOTIFICATION_MANAGER_H
