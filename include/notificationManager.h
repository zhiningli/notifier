#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include "notification.h"
#include "windows_api.h"
#include <string>
#include <unordered_map>
#include <set>
#include <vector>
#include <iostream>
#include <mutex>
#include <thread>
#include <atomic>

class NotificationManager {
public:
    static NotificationManager& getInstance();

    void addNotification(const Notification& notification);
    void removeNotification(const std::string& sessionID);
    void displayNotification(const std::string& notificationID);
    void displayAllNotifications();


    ~NotificationManager();

private:


    std::unordered_map<std::string, std::shared_ptr<Notification>> notifications;
    std::mutex managerMutex;

    WindowsAPI windowsAPI;

    std::atomic<bool> stopExpiryChecker;
    std::thread expiryCheckerThread;

    NotificationManager();
    NotificationManager(const NotificationManager&) = delete;
    NotificationManager& operator=(const NotificationManager&) = delete;
};

#endif  // NOTIFICATION_MANAGER_H
