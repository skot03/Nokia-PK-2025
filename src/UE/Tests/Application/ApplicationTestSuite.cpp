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
}
