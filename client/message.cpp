#include "message.h"
using namespace  std;

Message::Message(const int& id, const string& text, const int& from, const int& to, const int& status, const time_t& dateTime) :
    m_id(id), m_text(text), m_from(from), m_to(to), m_status(status), m_dateTime(dateTime) {
}
const int& Message::getId() const {
    return m_id;
}
const string& Message::getText() const {
    return m_text;
}
const int& Message::getFrom() const {
    return m_from;
}
const int& Message::getTo() const {
    return m_to;
}
const int& Message::getStatus() const {
    return m_status;
}
const std::time_t& Message::getTime() const {
    return m_dateTime;
}
