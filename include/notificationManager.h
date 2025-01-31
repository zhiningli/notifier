#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include "notification.h"
#include "windows_api.h"
#include <string>
#include <unordered_map>
#include <set>
#include <vector>
#include <iostream>
#include <mutex> // For thread safety

class NotificationManager {
public:
	// Singleton pattern for instaitiating the NotificationManager only once
	static NotificationManager& getInstance();

	void addNotification(const Notification& notification);
	void displayAllNotifications();
	void displayLatestNotification();
	void removeExpiredNotifications();
	void NotificationManager::updateNotificationExpiry(const std::string& sessionID, const std::chrono::system_clock::time_point& newExpiry);
	~NotificationManager();

private:

	struct ExpiryComparator {
		bool operator()(const Notification* a, const Notification* b) const {
			return a->getExpiryTime() < b->getExpiryTime();
		}
	};

	std::unordered_map<std::string, Notification> notifications;
	std::multiset<Notification*, ExpiryComparator> expirySet; // For sort order by expiry time
	std::mutex managerMutex; // For thread safety
	
	WindowsAPI windowsAPI; // For displaying toast notifications

	NotificationManager() = default;
	NotificationManager(const NotificationManager&) = delete;
	NotificationManager& operator=(const NotificationManager&) = delete;
};

#endif
