#pragma once
#include <string>

class User { // Пользователь
public:
	User(const std::string&, const std::string&, const std::string&);
	~User() = default;
	void setUserPassword(const std::string&);
	void setUserName(const std::string&);
	const std::string& getUserLogin() const;
	const std::string& getUserPassword();
	const std::string& getUserName() const;
private:
	std::string m_login;
	std::string m_password;
	std::string m_name;
};
