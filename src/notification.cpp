#include "notification.h"
#include <iostream>
#include <chrono>


Notification::Notification(const std::string& title,
    const std::string& message,
    const std::string& sourceID,
    const std::string& sessionID,
    SourceEnum source,
    StatusEnum status,
    std::chrono::system_clock::time_point expiryTime)
    : _title(title),
    _message(message),
    _sourceID(sourceID),
    _sessionID(sessionID),
    _source(source),
    _status(status),
    _expiryTime(expiryTime),
    _creationTime(std::chrono::system_clock::now()) {
}

Notification::Notification(const std::string& title,
    const std::string& message,
    const std::string& sourceID,
    const std::string& sessionID,
    SourceEnum source,
    std::chrono::system_clock::time_point expiryTime)
    : _title(title),
    _message(message),
    _sourceID(sourceID),
    _sessionID(sessionID),
    _source(source),
    _status(StatusEnum::Active),
    _expiryTime(expiryTime),
    _creationTime(std::chrono::system_clock::now()) {
}


// check if notification is expired and set the status accordingly
bool Notification::isExpired() {
    auto currentTime = std::chrono::system_clock::now();
    if (currentTime > _expiryTime) {
        _status = StatusEnum::Expired;
        return true;
    }
    else {
        _status = StatusEnum::Active;
        return false;
    }
}

// Getters
const std::string& Notification::getTitle() const { return _title; }
const std::string& Notification::getMessage() const { return _message; }
const std::string& Notification::getSourceID() const { return _sourceID; }
const std::string& Notification::getSessionID() const { return _sessionID; }
Notification::SourceEnum Notification::getSource() const { return _source; }
Notification::StatusEnum Notification::getStatus() const { return _status; }
std::chrono::system_clock::time_point Notification::getCreationTime() const { return _creationTime; }
std::chrono::system_clock::time_point Notification::getExpiryTime() const { return _expiryTime; }

// Setters
void Notification::setTitle(const std::string& title) { _title = title; }
void Notification::setMessage(const std::string& message) { _message = message; }
void Notification::setSourceID(const std::string& sourceID) { _sourceID = sourceID; }
void Notification::setSessionID(const std::string& sessionID) { _sessionID = sessionID; }
void Notification::setSource(SourceEnum source) { _source = source; }
void Notification::setStatus(StatusEnum status) { _status = status; }
void Notification::setExpiryTime(const std::chrono::system_clock::time_point& expiryTime) { _expiryTime = expiryTime; }


