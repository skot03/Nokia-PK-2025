#pragma once

#include <string>
#include <vector>
#include <optional>
#include "Messages/PhoneNumber.hpp"

namespace ue {

struct Sms {
    enum class SmsDirection {
        Sent,
        Received
    };
    
    enum class SmsStatus {
        Unread,
        Read
    };
    
    common::PhoneNumber phoneNumber;
    std::string text;
    SmsDirection direction;
    SmsStatus status;

    Sms(common::PhoneNumber from, std::string text)
       : phoneNumber(from), text(text), direction(SmsDirection::Received), status(SmsStatus::Unread)
    {}

    Sms(common::PhoneNumber from, std::string text, SmsStatus initialStatus)
        : phoneNumber(from), text(text), direction(SmsDirection::Sent), status(initialStatus)
    {}
};

class SmsDb {
public:
    SmsDb() = default;

    std::size_t addSMS(common::PhoneNumber from ,const std::string& text);
    std::size_t addSentSMS(common::PhoneNumber from, const std::string& text, Sms::SmsStatus initialStatus = Sms::SmsStatus::Unread);
    const std::vector<Sms>& getAllSMS() const;
    std::size_t getUnread() const;
    bool markAsRead(std::size_t idx);
    std::vector<Sms>::iterator begin();
	std::vector<Sms>::iterator end();

private:
    std::optional<std::size_t> lastSmsIndex;
    std::vector<Sms> messages;
};
}