#include "notification.h"

Notification::Notification(const std::string& title,
    const std::string& message,
    const std::string& notificationID,
    const std::string& sessionID,
    StatusEnum status,
    std::chrono::system_clock::time_point creationTime)
    : _title(title), _message(message), _notificationID(notificationID),
    _sessionID(sessionID), _status(status), _creationTime(creationTime) {
}

const std::string& Notification::getTitle() const { return _title; }
const std::string& Notification::getMessage() const { return _message; }
const std::string& Notification::getNotificationID() const { return _notificationID; }
const std::string& Notification::getSessionID() const { return _sessionID; }
StatusEnum Notification::getStatus() const { return _status; }
std::chrono::system_clock::time_point Notification::getCreationTime() const { return _creationTime; }


void Notification::setTitle(const std::string& title) { _title = title; };
void Notification::setMessage(const std::string& message) { _message = message; };


NotificationBuilder& NotificationBuilder::setTitle(const std::string& title) {
    _title = title;
    return *this;
}

NotificationBuilder& NotificationBuilder::setMessage(const std::string& message) {
    _message = message;
    return *this;
}

NotificationBuilder& NotificationBuilder::setNotificationID(const std::string& notificationID) {
    _notificationID = notificationID;
    return *this;
}

NotificationBuilder& NotificationBuilder::setSessionID(const std::string& sessionID) {
    _sessionID = sessionID;
    return *this;
}

NotificationBuilder& NotificationBuilder::setStatus(StatusEnum status) {
    _status = status;
    return *this;
}

NotificationBuilder& NotificationBuilder::setCreationTime(std::chrono::system_clock::time_point creationTime) {
    _creationTime = creationTime;
    return *this;
}

Notification NotificationBuilder::build() const {
    return Notification(_title, _message, _notificationID, _sessionID, _status, _creationTime);
}
