#pragma once
#include <string>
#include <ctime>

class Message { // Сообщение
public:
	Message(const int& id, const std::string& text, const int& from, const int& to, const int& status, const std::time_t& dateTime);
	~Message() = default;
	const int& getId() const;
	const std::string& getText() const;
	const int& getFrom() const;
	const int& getTo() const;
	const int& getStatus() const;
	const std::time_t& getTime() const;
private:
	int m_id;
	std::string m_text;
	int m_from;
	int m_to;
	int m_status;
	std::time_t m_dateTime;
};
