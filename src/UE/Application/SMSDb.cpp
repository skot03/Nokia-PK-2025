#include "SMSDb.hpp"
#include "Messages/PhoneNumber.hpp"
#include <map>
#include <string>
#include <vector>
#include <optional>

namespace ue{

    std::size_t addSMS(common::PhoneNumber from, const std::string& text) {
        //TODO
        return 0;
    }

    std::size_t addSentSMS(common::PhoneNumber to, const std::string& text, Sms::SmsStatus initialStatus = Sms::SmsStatus::Unread) {
        //TODO
        return 0;
    }

    const std::vector<Sms>& getAllSMS() {
        //TODO
    }

    std::size_t getUnread() {
        //TODO
        return 0;
    }

    bool markAsRead(std::size_t idx) {
        //TODO
        return 0;
    }
}