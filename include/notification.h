#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>
#include <chrono>

enum class StatusEnum {
    Active,
    Expired,
    Unknown,
};

enum class SourceEnum {
    Python,
    Cpp,
    Unknown,
};

class Notification {
public:
    Notification(const std::string& title,
        const std::string& message,
        const std::string& sourceID,
        const std::string& sessionID,
        SourceEnum source,
        StatusEnum status,
        std::chrono::system_clock::time_point creationTime);

    // Getters
    const std::string& getTitle() const;
    const std::string& getMessage() const;
    const std::string& getSourceID() const;
    const std::string& getSessionID() const;
    SourceEnum getSource() const;
    StatusEnum getStatus() const;
    std::chrono::system_clock::time_point getCreationTime() const;

private:
    std::string _title;
    std::string _message;
    std::string _sourceID;
    std::string _sessionID;
    SourceEnum _source;
    StatusEnum _status;
    std::chrono::system_clock::time_point _creationTime;
};

class NotificationBuilder {
public:
    NotificationBuilder& setTitle(const std::string& title);
    NotificationBuilder& setMessage(const std::string& message);
    NotificationBuilder& setSourceID(const std::string& sourceID);
    NotificationBuilder& setSessionID(const std::string& sessionID);
    NotificationBuilder& setSource(SourceEnum source);
    NotificationBuilder& setStatus(StatusEnum status);
    NotificationBuilder& setCreationTime(std::chrono::system_clock::time_point creationTime);

    Notification build() const;

private:
    std::string _title = "Default Title";
    std::string _message = "Default Message";
    std::string _sourceID = "Default SourceID";
    std::string _sessionID = "Default SessionID";
    SourceEnum _source = SourceEnum::Unknown;
    StatusEnum _status = StatusEnum::Unknown;
    std::chrono::system_clock::time_point _creationTime = std::chrono::system_clock::now();
};

#endif // NOTIFICATION_H