#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Application.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Mocks/IUserPortMock.hpp"
#include "Mocks/ITimerPortMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include "States/ConnectingState.hpp"
#include "States/ConnectedState.hpp"
#include "States/NotConnectedState.hpp"
#include "States/TalkingState.hpp"
#include "States/DialState.hpp"
#include "Mocks/IUeGuiMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include "Messages/PhoneNumber.hpp"
#include "States/ComposeSmsState.hpp"
#include "States/ViewSmsListState.hpp"
#include "States/ViewSmsState.hpp"
#include <memory>

namespace ue
{
using namespace ::testing;
using namespace std::chrono_literals;

struct ApplicationTestSuite : Test
{
    const common::PhoneNumber PHONE_NUMBER{112};
    //const common::BtsId BTS_ID{22};
    const common::BtsId BTS_ID{1024};
    NiceMock<common::ILoggerMock> loggerMock;
    NiceMock<IBtsPortMock> btsPortMock;
    NiceMock<IUserPortMock> userPortMock;
    NiceMock<ITimerPortMock> timerPortMock;

    //Expectation showNotConnected = EXPECT_CALL(userPortMock, showNotConnected());
    Application objectUnderTest{PHONE_NUMBER,
                                loggerMock,
                                btsPortMock,
                                userPortMock,
                                timerPortMock};
};

struct ApplicationNotConnectedTestSuite : ApplicationTestSuite
{
    void sendAttachRequestOnSib();
};

void ApplicationNotConnectedTestSuite::sendAttachRequestOnSib()
{
    ON_CALL(timerPortMock, startTimer(_)).WillByDefault(Return());
    ON_CALL(timerPortMock, stopTimer()).WillByDefault(Return());
    ON_CALL(userPortMock, showConnecting()).WillByDefault(Return());

    EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID)).Times(1);
    EXPECT_CALL(timerPortMock, startTimer(500ms)).Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showConnecting()).Times(AtLeast(1));

    objectUnderTest.handleSib(BTS_ID);
}

TEST_F(ApplicationNotConnectedTestSuite, shallSendAttachRequestOnSib)
{
    sendAttachRequestOnSib();
}

struct ApplicationConnectingTestSuite : ApplicationNotConnectedTestSuite
{
    ApplicationConnectingTestSuite()
    {
        sendAttachRequestOnSib();
    }
};

TEST_F(ApplicationConnectingTestSuite, shallConnectOnAttachAccept)
{
    EXPECT_CALL(timerPortMock, stopTimer()).Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showConnected()).Times(1);
    objectUnderTest.handleAttachAccept();
}

TEST_F(ApplicationConnectingTestSuite, shallDisConnectOnAttachReject)
{
    EXPECT_CALL(timerPortMock, stopTimer()).Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showNotConnected()).Times(AtLeast(1));
    objectUnderTest.handleAttachReject();
}

TEST_F(ApplicationConnectingTestSuite, shallDisConnectOnTimeout)
{
    EXPECT_CALL(userPortMock, showNotConnected()).Times(AtLeast(1));
    objectUnderTest.handleTimeout();
}

TEST_F(ApplicationTestSuite, shouldEnterConnectingState)
{
    ON_CALL(timerPortMock, stopTimer()).WillByDefault(Return());
    ON_CALL(timerPortMock, startTimer(_)).WillByDefault(Return());
    ON_CALL(userPortMock, showConnecting()).WillByDefault(Return());

    EXPECT_CALL(userPortMock, showConnecting()).Times(AtLeast(1));
    EXPECT_CALL(timerPortMock, startTimer(500ms)).Times(AtLeast(1));

    objectUnderTest.context.setState<ConnectingState>();
}

TEST_F(ApplicationTestSuite, shallHandleDisconnectedInConnectedState)
{
    objectUnderTest.getContext().setState<ConnectedState>();
    EXPECT_CALL(userPortMock, showNotConnected()).Times(AtLeast(1));
    objectUnderTest.handleDisconnected();
}
TEST_F(ApplicationTestSuite, shouldRejectIncomingCallInConnectedState)
{
    objectUnderTest.getContext().setState<ConnectedState>();

    const common::PhoneNumber caller{123};
    EXPECT_CALL(btsPortMock, sendCallDropped(caller)).Times(1);
    EXPECT_CALL(userPortMock, showCallRequest(caller)).Times(1);
    EXPECT_CALL(userPortMock, rejectCallback(_)).WillOnce(Invoke([](auto reject){
        reject();  // symulacja kliknięcia "Odrzuć"
    }));

    objectUnderTest.handleCallMessage(common::MessageId::CallRequest, caller);
}
/*Szybkie sprawdzenie poniższego testu wymaga zmianę
modyfikatora dostępu metody getContext() z pliku Application.hpp na public.
Obecnie metoda ustawiona jest jako public*/
TEST_F(ApplicationTestSuite, shallReconnectAfterDisconnect)
{
    objectUnderTest.getContext().setState<ConnectedState>();

    EXPECT_CALL(userPortMock, showNotConnected()).Times(AtLeast(1));
    objectUnderTest.handleDisconnected();

    ON_CALL(timerPortMock, startTimer(_)).WillByDefault(Return());
    ON_CALL(timerPortMock, stopTimer()).WillByDefault(Return());
    ON_CALL(userPortMock, showConnecting()).WillByDefault(Return());

    EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID)).Times(1);
    EXPECT_CALL(timerPortMock, startTimer(500ms)).Times(AtLeast(1));
    EXPECT_CALL(userPortMock, showConnecting()).Times(AtLeast(1));

    objectUnderTest.handleSib(BTS_ID);
}

struct ApplicationDialStateTestSuite : Test 
{
    const common::PhoneNumber PHONE_NUMBER{112}; 
    const common::PhoneNumber DIALED_PHONE_NUMBER{200}; 
    const common::PhoneNumber OTHER_CALLER_ID{150};  
    const common::BtsId BTS_ID{1024};

    NiceMock<common::ILoggerMock> loggerMock;
    NiceMock<IBtsPortMock> btsPortMock;
    NiceMock<IUserPortMock> userPortMock;
    NiceMock<ITimerPortMock> timerPortMock;
    NiceMock<IDialModeMock> dialModeMock;   
    NiceMock<ICallModeMock> callModeMock;   

    Application objectUnderTest{PHONE_NUMBER,
                                 loggerMock,
                                 btsPortMock,
                                 userPortMock,
                                 timerPortMock};

    std::function<void()> capturedDialStateHomeCallback;
    std::function<void()> capturedDialStateAcceptCallback;
    std::function<void()> capturedIncomingCallAcceptCallback;
    std::function<void()> capturedIncomingCallRejectCallback;

    ApplicationDialStateTestSuite()
    {
        ON_CALL(timerPortMock, startTimer(_)).WillByDefault(Return());
        ON_CALL(timerPortMock, stopTimer()).WillByDefault(Return());
        ON_CALL(userPortMock, showNotConnected()).WillByDefault(Return());
        ON_CALL(userPortMock, showConnecting()).WillByDefault(Return());
        ON_CALL(userPortMock, showConnected()).WillByDefault(Return());
        ON_CALL(btsPortMock, sendAttachRequest(_)).WillByDefault(Return());
        ON_CALL(loggerMock, log(testing::_, testing::_)).WillByDefault(Return());


        // Dołączenie do sieci: NotConnected -> Connecting -> Connected
        EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID)).Times(AtLeast(1));
        EXPECT_CALL(timerPortMock, startTimer(500ms)).Times(AtLeast(1));
        EXPECT_CALL(userPortMock, showConnecting()).Times(AtLeast(1));
        objectUnderTest.handleSib(BTS_ID);
        Mock::VerifyAndClearExpectations(&btsPortMock);
        Mock::VerifyAndClearExpectations(&timerPortMock);
        Mock::VerifyAndClearExpectations(&userPortMock);

        EXPECT_CALL(timerPortMock, stopTimer()).Times(AtLeast(1));
        EXPECT_CALL(userPortMock, showConnected()).Times(AtLeast(1));
        objectUnderTest.handleAttachAccept();
        Mock::VerifyAndClearExpectations(&timerPortMock);
        Mock::VerifyAndClearExpectations(&userPortMock);

        ON_CALL(dialModeMock, getPhoneNumber()).WillByDefault(Return(DIALED_PHONE_NUMBER));

        EXPECT_CALL(userPortMock, activateDialMode())
            .WillOnce(ReturnRef(dialModeMock));
        EXPECT_CALL(userPortMock, homeCallback(_))
            .WillOnce(SaveArg<0>(&capturedDialStateHomeCallback));
        EXPECT_CALL(userPortMock, acceptCallback(_))
            .WillOnce(SaveArg<0>(&capturedDialStateAcceptCallback));

        objectUnderTest.getContext().setState<DialState>();
        Mock::VerifyAndClearExpectations(&userPortMock);
    }

    ~ApplicationDialStateTestSuite() override = default;
};

// Test 1: Naciśnięcie przycisku "Wybierz" (acceptCallback)
TEST_F(ApplicationDialStateTestSuite, DialButton_ShouldSendCallRequestAndCallGetPhoneNumberTwice)
{
    ASSERT_TRUE(capturedDialStateAcceptCallback) << "DialState accept callback nie został przechwycony.";
    EXPECT_CALL(dialModeMock, getPhoneNumber())
        .Times(2)
        .WillRepeatedly(Return(DIALED_PHONE_NUMBER));

    EXPECT_CALL(btsPortMock, sendCallRequest(DIALED_PHONE_NUMBER)).Times(1);

    capturedDialStateAcceptCallback();
}

// Test 2: Naciśnięcie przycisku "Home" w DialState (homeCallback)
TEST_F(ApplicationDialStateTestSuite, HomeButtonInDialState_ShouldShowConnectedScreen)
{
    ASSERT_TRUE(capturedDialStateHomeCallback) << "DialState home callback nie został przechwycony.";
    EXPECT_CALL(userPortMock, showConnected()).Times(1);
    capturedDialStateHomeCallback();
}

// Test 3: Obsługa komunikatu CallAccepted
TEST_F(ApplicationDialStateTestSuite, HandleCallMessage_CallAccepted_ShouldStopTimerAndEnterTalkingState)
{
    EXPECT_CALL(timerPortMock, stopTimer()).Times(1);

    
    EXPECT_CALL(userPortMock, activateCallMode()).WillOnce(ReturnRef(callModeMock));
    EXPECT_CALL(callModeMock, clearIncomingText()).Times(1);
    EXPECT_CALL(callModeMock, clearOutgoingText()).Times(1);
    EXPECT_CALL(userPortMock, acceptCallback(_)).Times(1); 
    EXPECT_CALL(userPortMock, rejectCallback(_)).Times(1); 
    EXPECT_CALL(userPortMock, setCloseGuard(_)).Times(1);  

    objectUnderTest.handleCallMessage(common::MessageId::CallAccepted, DIALED_PHONE_NUMBER);
    
}

// Test 4: Obsługa komunikatu CallDropped
TEST_F(ApplicationDialStateTestSuite, HandleCallMessage_CallDropped_ShouldStopTimerShowDroppedAndEnterConnectedState)
{
    EXPECT_CALL(timerPortMock, stopTimer()).Times(1);
    EXPECT_CALL(userPortMock, showCallDropped()).Times(1);
    
    objectUnderTest.handleCallMessage(common::MessageId::CallDropped, DIALED_PHONE_NUMBER);
}

// Test 5: Obsługa komunikatu UnknownRecipient
TEST_F(ApplicationDialStateTestSuite, HandleCallMessage_UnknownRecipient_ShouldStopTimerShowNotAvailableAndEnterConnectedState)
{
    EXPECT_CALL(timerPortMock, stopTimer()).Times(1);
    
    EXPECT_CALL(dialModeMock, getPhoneNumber()).WillOnce(Return(DIALED_PHONE_NUMBER));
    EXPECT_CALL(userPortMock, showPeerUserNotAvailable(DIALED_PHONE_NUMBER)).Times(1);
    objectUnderTest.handleCallMessage(common::MessageId::UnknownRecipient, DIALED_PHONE_NUMBER);
}


struct ApplicationSmsStateTestSuite : Test {
    const common::PhoneNumber PHONE_NUMBER{111};
    const common::PhoneNumber SENDER{123};
    const common::PhoneNumber RECEIVER{222};
    const common::BtsId BTS_ID{1};
    const std::string MESSAGE_TEXT = "text";

    NiceMock<common::ILoggerMock> loggerMock;
    NiceMock<IBtsPortMock> btsPortMock;
    NiceMock<IUserPortMock> userPortMock;
    NiceMock<ITimerPortMock> timerPortMock;

    Application app{PHONE_NUMBER, loggerMock, btsPortMock, userPortMock, timerPortMock};

    ApplicationSmsStateTestSuite()
    {
        // Symulacja podłączenia do sieci
        EXPECT_CALL(userPortMock, showConnecting()).Times(1);
        EXPECT_CALL(btsPortMock, sendAttachRequest(BTS_ID)).Times(1);
        app.handleSib(BTS_ID);

        EXPECT_CALL(userPortMock, showConnected()).Times(AtLeast(1));
        app.handleAttachAccept();
    }
};

// Test: Wysłanie SMS-a
TEST_F(ApplicationSmsStateTestSuite, ShouldComposeAndSendSms)
{
    EXPECT_CALL(userPortMock, composeSMS()).Times(1);
    app.getContext().setState<ComposeSmsState>();

    EXPECT_CALL(btsPortMock, sendSms(RECEIVER, MESSAGE_TEXT)).Times(1);
    EXPECT_CALL(userPortMock, showConnected()).Times(AtLeast(1));

    app.handleSendSms(RECEIVER, MESSAGE_TEXT);

    const auto& sms = app.getContext().smsDb.getAllSMS().front();
    EXPECT_EQ(sms.text, MESSAGE_TEXT);
    EXPECT_EQ(sms.phoneNumber, RECEIVER);
    EXPECT_EQ(sms.direction, Sms::SmsDirection::Sent);
}

// Test: Odbieranie SMS-a i zapis w bazie
TEST_F(ApplicationSmsStateTestSuite, ShouldReceiveAndStoreIncomingSms)
{
    EXPECT_CALL(userPortMock, showNewSmsNotification()).Times(1);
    app.handleReceiveSMS(common::MessageId::Sms, SENDER, MESSAGE_TEXT);

    const auto& sms = app.getContext().smsDb.getAllSMS().front();
    EXPECT_EQ(sms.text, MESSAGE_TEXT);
    EXPECT_EQ(sms.phoneNumber, SENDER);
    EXPECT_EQ(sms.direction, Sms::SmsDirection::Received);
    EXPECT_EQ(sms.status, Sms::SmsStatus::Unread);
}

// Test: Wyświetlanie listy SMS
TEST_F(ApplicationSmsStateTestSuite, ShouldDisplaySmsList)
{
    app.getContext().smsDb.addSMS(SENDER, "msg1");
    app.getContext().smsDb.addSMS(PHONE_NUMBER, "msg2");

    EXPECT_CALL(userPortMock, showSmsList(_)).WillOnce(Invoke([](const SmsDb& db){
        const auto& list = db.getAllSMS();
        ASSERT_EQ(list.size(), 2);
        EXPECT_EQ(list[0].text, "msg2");
        EXPECT_EQ(list[1].text, "msg1");
    }));

    app.getContext().setState<ViewSmsListState>();
}

// Test: Wyświetlanie pojedynczej wiadomości
TEST_F(ApplicationSmsStateTestSuite, ShouldDisplaySingleSms)
{
    app.getContext().smsDb.addSMS(SENDER, "text");

    EXPECT_CALL(userPortMock, showSMS(_)).WillOnce(Invoke([](const Sms& sms) {
        EXPECT_EQ(sms.text, "text");
        EXPECT_EQ(sms.phoneNumber, common::PhoneNumber{123});
    }));

    app.getContext().setState<ViewSmsState>(0);
}

  }
