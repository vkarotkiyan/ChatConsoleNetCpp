#pragma once
#include <string>

class User { // Пользователь
public:
	User(const int&, const std::string&, const std::string&, const std::string&, const int&);
	~User() = default;
	void setUserPassword(const std::string&);
	void setUserName(const std::string&);
	const int& getUserId() const;
	const std::string& getUserLogin() const;
	const std::string& getUserPassword();
	const std::string& getUserName() const;
	const int& getUserStatus() const;
private:
	int m_id = 0;
	std::string m_login;
	std::string m_password;
	std::string m_name;
	int m_status = '0';
};
