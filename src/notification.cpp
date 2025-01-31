#include "notification.h"
#include <iostream>
#include <chrono>


Notification::Notification()
	: title(""), message(""), sourceID(""), sessionID(""), source(SourceEnum::Unknown), status(StatusEnum::Unknown), creationTime(std::chrono::system_clock::now()) {}

Notification::Notification(const std::string &title, 
							const std::string &message, 
							const std::string &sourceID, 
							const std::string &sessionID, 
							SourceEnum source, 
							StatusEnum status)
	: title(title), message(message), sourceID(sourceID), sessionID(sessionID), source(source), status(status),
	creationTime(std::chrono::system_clock::now()) {}

Notification::Notification(const std::string& title,
	const std::string& message,
	const std::string& sourceID,
	const std::string& sessionID,
	SourceEnum source)
	: title(title), message(message), sourceID(sourceID), sessionID(sessionID), source(source), status(StatusEnum::Active),
	creationTime(std::chrono::system_clock::now()) {};


// Getter and Setter
const std::string& Notification::getTitle() const {
	return title;
}

const std::string& Notification::getMessage() const {
	return message;
}

const std::string& Notification::getSourceID() const {
	return sourceID;
}

const std::string& Notification::getSessionID() const {
	return sessionID;
}

const Notification::SourceEnum Notification::getSource() const {
	return source;
}

const Notification::StatusEnum Notification::getStatus() const {
	return status;
}

const std::chrono::system_clock::time_point Notification::getCreationTime() const
{
	return creationTime;
}

const std::chrono::system_clock::time_point Notification::getExpiryTime() const {
	return expiryTime;
}

void Notification::setTitle(const std::string& title) {
	this->title = title;
}

void Notification::setMessage(const std::string& message) {
	this->message = message;
}

void Notification::setSourceID(const std::string& sourceID) {
	this->sourceID = sourceID;
}

void Notification::setSessionID(const std::string& sessionID) {
	this->sessionID = sessionID;
}

void Notification::setSource(SourceEnum source) {
	this->source = source;
}

void Notification::setStatus(StatusEnum status) {
	this->status = status;
}

void Notification::setExpiryTime(const std::chrono::system_clock::time_point& expiryTime) {
	this->expiryTime = expiryTime;
	this->isExpired();
}


// check if notification is expired and set the status accordingly
bool Notification::isExpired() {
	auto currentTime = std::chrono::system_clock::now();
	if (currentTime > expiryTime) {
		status = StatusEnum::Expired;
		return true;
	}
	else {
		status = StatusEnum::Active;
		return false;
	}
}