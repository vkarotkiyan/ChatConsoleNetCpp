#pragma once
#include <string>
#include <ctime>

class Message { // Сообщение
public:
	Message(const std::string & text, const std::string& from, const std::string& to, const std::time_t& dateTime);
	~Message() = default;
	const std::string& getText() const;
	const std::string& getFrom() const;
	const std::string& getTo() const;
	const std::time_t& getTime() const;
private:
	std::string m_text;
	std::string m_from;
	std::string m_to;
	std::time_t m_dateTime;
};
