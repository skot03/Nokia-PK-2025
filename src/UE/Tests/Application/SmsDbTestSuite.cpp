#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "SMSDb.hpp"
#include "Messages/PhoneNumber.hpp"

namespace ue {

using namespace ::testing;

struct SmsDbTestSuite : Test
{
    SmsDb db;
    const common::PhoneNumber NUMBER_1{100};
    const common::PhoneNumber NUMBER_2{101};
};

// Test sprawdza, czy odebrany SMS jest poprawnie zapisany
TEST_F(SmsDbTestSuite, shouldAddReceivedSms)
{
    db.addSMS(NUMBER_1, "text");
    const auto& all = db.getAllSMS();

    ASSERT_EQ(all.size(), 1);
    EXPECT_EQ(all[0].text, "text");
    EXPECT_EQ(all[0].phoneNumber, NUMBER_1);
    EXPECT_EQ(all[0].status, Sms::SmsStatus::Unread);
    EXPECT_EQ(all[0].direction, Sms::SmsDirection::Received);
}

// Test sprawdza, czy wysłany SMS jest zapisany ze wskazanym statusem
TEST_F(SmsDbTestSuite, shouldAddSentSmsWithStatus)
{
    db.addSentSMS(NUMBER_2, "text", Sms::SmsStatus::Read);
    const auto& sms = db.getAllSMS().front();

    EXPECT_EQ(sms.text, "text");
    EXPECT_EQ(sms.direction, Sms::SmsDirection::Sent);
    EXPECT_EQ(sms.status, Sms::SmsStatus::Read);
}


// Test sprawdza zapis wielu wiadomości od tego samego nadawcy
TEST_F(SmsDbTestSuite, shouldSupportMultipleSmsFromSameSender)
{
    db.addSMS(NUMBER_1, "msg1");
    db.addSMS(NUMBER_1, "msg2");

    const auto& all = db.getAllSMS();
    ASSERT_EQ(all.size(), 2);
    EXPECT_EQ(all[0].phoneNumber, NUMBER_1);
    EXPECT_EQ(all[0].text, "msg2");
    EXPECT_EQ(all[1].text, "msg1");
}

// Test sprawdza, czy nowy SMS trafia na początek listy
TEST_F(SmsDbTestSuite, shouldStoreLatestMessageAtFront)
{
    db.addSMS(NUMBER_1, "msg1");
    db.addSMS(NUMBER_2, "msg2");

    const auto& all = db.getAllSMS();
    EXPECT_EQ(all.front().text, "msg2");
    EXPECT_EQ(all.back().text, "msg1");
}

}
