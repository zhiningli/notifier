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
    void displayAllNotifications();
    void displayLatestNotification();
    void updateExpiredNotifications();
    void updateNotificationExpiry(const std::string& sessionID,
        const std::chrono::system_clock::time_point& newExpiry);

    ~NotificationManager();

private:
    struct ExpiryComparator {
        bool operator()(const std::shared_ptr<Notification>& a,
            const std::shared_ptr<Notification>& b) const {
            return a->getExpiryTime() < b->getExpiryTime();
        }
    };


    std::unordered_map<std::string, std::shared_ptr<Notification>> notifications;
    std::multiset<std::shared_ptr<Notification>, ExpiryComparator> expirySet;
    std::mutex managerMutex;

    WindowsAPI windowsAPI;

    std::atomic<bool> stopExpiryChecker;
    std::thread expiryCheckerThread;
    void expiryChecker();

    NotificationManager();
    NotificationManager(const NotificationManager&) = delete;
    NotificationManager& operator=(const NotificationManager&) = delete;
};

#endif  // NOTIFICATION_MANAGER_H
