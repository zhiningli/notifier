#ifndef NOTIFICATION.H
#define NOTIFICATION.H	

#include <string>
#include <iostream>
#include <chrono>


class Notification {
public:

	// Enum for status of a notificatoin
	enum class StatusEnum {
		Active,
		Expired,
		Unknown,
	};

	// Enum for the sourece of notification
	enum class SourceEnum {
		Python,
		Cpp,
		Unknown,
	};

	Notification();
	Notification(const std::string &title, 
				const std::string &message, 
				const std::string &sourceID,
				const std::string &sessionID,
				SourceEnum source,
				StatusEnum status );

	Notification(const std::string& title,
				const std::string& message,
				const std::string& sourceID,
				const std::string& sessionID,
				SourceEnum source);	
				

	// Getter and Setter
	const std::string& getTitle() const;
	const std::string& getMessage() const;
	const std::string& getSourceID() const;
	const std::string& getSessionID() const;
	const SourceEnum getSource() const;
	const StatusEnum getStatus() const;
	const std::chrono::system_clock::time_point getCreationTime() const;

	void setTitle(const std::string& title);
	void setMessage(const std::string& message);
	void setSourceID(const std::string& sourceID);
	void setSessionID(const std::string& sessionID);
	void setSource(SourceEnum source);
	void setStatus(StatusEnum status);

private:
	std::string title;
	std::string message;
	std::string sourceID;
	std::string sessionID;
	SourceEnum source;
	StatusEnum status;
	std::chrono::system_clock::time_point creationTime;
};

#endif