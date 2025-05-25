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
TEST_F(ApplicationDialStateTestSuite, DialButton_ShouldSendCallRequestStartTimerAndCallGetPhoneNumberTwice)
{
    ASSERT_TRUE(capturedDialStateAcceptCallback) << "DialState accept callback nie został przechwycony.";

    // Oczekujemy dwukrotnego wywołania getPhoneNumber w DialState::sendCallRequest(),raz loguje, raz wysyła żądanie
    EXPECT_CALL(dialModeMock, getPhoneNumber())
        .Times(2) 
        .WillRepeatedly(Return(DIALED_PHONE_NUMBER)); 
    
    EXPECT_CALL(btsPortMock, sendCallRequest(DIALED_PHONE_NUMBER)).Times(1);
    EXPECT_CALL(timerPortMock, startTimer(1000ms)).Times(1);

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

  }