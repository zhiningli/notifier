#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>
#include <iostream>
#include <chrono>

class Notification {
public:

	// Enum for status of a notification
	enum class StatusEnum {
		Active,
		Expired,
		Unknown,
	};

	// Enum for the source of the notification
	enum class SourceEnum {
		Python,
		Cpp,
		Unknown,
	};

	Notification();
	Notification(const std::string& title,
		const std::string& message,
		const std::string& sourceID,
		const std::string& sessionID,
		SourceEnum source,
		StatusEnum status,
		std::chrono::system_clock::time_point expiryTime);

	Notification(const std::string& title,
		const std::string& message,
		const std::string& sourceID,
		const std::string& sessionID,
		SourceEnum source,
		std::chrono::system_clock::time_point expiryTime);

	// Getter and Setter
	const std::string& getTitle() const;
	const std::string& getMessage() const;
	const std::string& getSourceID() const;
	const std::string& getSessionID() const;
	SourceEnum getSource() const;
	StatusEnum getStatus() const;
	std::chrono::system_clock::time_point getCreationTime() const;
	std::chrono::system_clock::time_point getExpiryTime() const;

	void setTitle(const std::string& title);
	void setMessage(const std::string& message);
	void setSourceID(const std::string& sourceID);
	void setSessionID(const std::string& sessionID);
	void setSource(SourceEnum source);
	void setStatus(StatusEnum status);
	void setExpiryTime(const std::chrono::system_clock::time_point& expiryTime);

	// Check if notification is expired
	bool isExpired();

private:
	std::string _title;
	std::string _message;
	std::string _sourceID;
	std::string _sessionID;
	SourceEnum _source;
	StatusEnum _status;
	std::chrono::system_clock::time_point _creationTime;
	std::chrono::system_clock::time_point _expiryTime;
};

#endif
