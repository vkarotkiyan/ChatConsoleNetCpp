#include "message.h"
using namespace  std;

Message::Message(const string& text, const string& from, const string& to, const time_t& dateTime) :
    m_text(text), m_from(from), m_to(to), m_dateTime(dateTime) {
}

const string& Message::getText() const {
    return m_text;
}

const string& Message::getFrom() const {
    return m_from;
}

const string& Message::getTo() const {
    return m_to;
}

const std::time_t& Message::getTime() const
{
    return m_dateTime;
}
