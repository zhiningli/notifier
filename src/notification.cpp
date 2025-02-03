#include "notification.h"

Notification::Notification(const std::string& title,
    const std::string& message,
    const std::string& sourceID,
    const std::string& sessionID,
    SourceEnum source,
    StatusEnum status,
    std::chrono::system_clock::time_point creationTime)
    : _title(title), _message(message), _sourceID(sourceID),
    _sessionID(sessionID), _source(source), _status(status), _creationTime(creationTime) {
}

const std::string& Notification::getTitle() const { return _title; }
const std::string& Notification::getMessage() const { return _message; }
const std::string& Notification::getSourceID() const { return _sourceID; }
const std::string& Notification::getSessionID() const { return _sessionID; }
SourceEnum Notification::getSource() const { return _source; }
StatusEnum Notification::getStatus() const { return _status; }
std::chrono::system_clock::time_point Notification::getCreationTime() const { return _creationTime; }

NotificationBuilder& NotificationBuilder::setTitle(const std::string& title) {
    _title = title;
    return *this;
}

NotificationBuilder& NotificationBuilder::setMessage(const std::string& message) {
    _message = message;
    return *this;
}

NotificationBuilder& NotificationBuilder::setSourceID(const std::string& sourceID) {
    _sourceID = sourceID;
    return *this;
}

NotificationBuilder& NotificationBuilder::setSessionID(const std::string& sessionID) {
    _sessionID = sessionID;
    return *this;
}

NotificationBuilder& NotificationBuilder::setSource(SourceEnum source) {
    _source = source;
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
    return Notification(_title, _message, _sourceID, _sessionID, _source, _status, _creationTime);
}
