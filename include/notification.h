#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>
#include <chrono>

enum class StatusEnum {
    Active,
    Expired,
    Unknown,
};

class Notification {
public:
    Notification(const std::string& title,
        const std::string& message,
        const std::string& notificationID,
        const std::string& sessionID,
        StatusEnum status,
        std::chrono::system_clock::time_point creationTime);

    // Getters
    const std::string& getTitle() const;
    const std::string& getMessage() const;
    const std::string& getNotificationID() const;
    const std::string& getSessionID() const;
    StatusEnum getStatus() const;
    std::chrono::system_clock::time_point getCreationTime() const;

    // Setters
    void setTitle(const std::string& title);
    void setMessage(const std::string& message);

private:
    std::string _title;
    std::string _message;
    std::string _notificationID;
    std::string _sessionID;
    StatusEnum _status;
    std::chrono::system_clock::time_point _creationTime;
};

class NotificationBuilder {
public:
    NotificationBuilder& setTitle(const std::string& title);
    NotificationBuilder& setMessage(const std::string& message);
    NotificationBuilder& setNotificationID(const std::string& notificationID);
    NotificationBuilder& setSessionID(const std::string& sessionID);
    NotificationBuilder& setStatus(StatusEnum status);
    NotificationBuilder& setCreationTime(std::chrono::system_clock::time_point creationTime);

    Notification build() const;

private:
    std::string _title = "Default Title";
    std::string _message = "Default Message";
    std::string _notificationID = "Default NotificationID";
    std::string _sessionID = "Default SessionID";
    StatusEnum _status = StatusEnum::Unknown;
    std::chrono::system_clock::time_point _creationTime = std::chrono::system_clock::now();
};

#endif // NOTIFICATION_H