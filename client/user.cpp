#include "user.h"
using namespace  std;

User::User(const string& login, const string& password, const string& name) :
    m_login(login), m_password(password), m_name(name) {
}
void User::setUserPassword(const string& password) {
    m_password = password;
}
void User::setUserName(const string& name) {
    m_name = name;
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
