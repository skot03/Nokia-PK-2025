#include "SMSDb.hpp"
#include <stdexcept>

namespace ue {

std::size_t SmsDb::addSMS(common::PhoneNumber from, const std::string& text) {
    messages.emplace_back(from, text);
    lastSmsIndex = messages.size() - 1;
    return *lastSmsIndex;
}

std::size_t SmsDb::addSentSMS(common::PhoneNumber to, const std::string& text, Sms::SmsStatus initialStatus) {
    messages.emplace_back(to, text, initialStatus); 
    lastSmsIndex = messages.size() - 1;
    return *lastSmsIndex;
}

const std::vector<Sms>& SmsDb::getAllSMS() const {
    return messages;
}

std::size_t SmsDb::getUnread() const {
    std::size_t count = 0;
    for (const auto& sms : messages) {
        if (sms.status == Sms::SmsStatus::Unread && sms.direction == Sms::SmsDirection::Received) {
            ++count;
        }
    }
    return count;
}

bool SmsDb::markAsRead(std::size_t idx) {
    if (idx >= messages.size()) {
        return false; 
    messages[idx].status = Sms::SmsStatus::Read;
    return true;
    }
}

}