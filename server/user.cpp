#include "user.h"
using namespace  std;

User::User(const int& id, const string& login, const string& password, const string& name, const int& status) :
    m_id(id), m_login(login), m_password(password), m_name(name), m_status(status) {
}
void User::setUserPassword(const string& password) {
    m_password = password;
}
void User::setUserName(const string& name) {
    m_name = name;
}
const int& User::getUserId() const {
    return m_id;
}
const string& User::getUserLogin() const {
    return m_login;
}
const string& User::getUserPassword() {
    return m_password;
}
const string& User::getUserName() const {
    return m_name;
}
const int& User::getUserStatus() const {
    return m_status;
}
