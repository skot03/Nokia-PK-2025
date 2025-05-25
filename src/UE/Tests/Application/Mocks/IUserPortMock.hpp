#pragma once

#include <gmock/gmock.h>
#include "Ports/IUserPort.hpp"

namespace ue
{

class IUserEventsHandlerMock : public IUserEventsHandler
{
public:
    IUserEventsHandlerMock();
    ~IUserEventsHandlerMock() override;
    MOCK_METHOD(void, handleViewSmsList, (), (final)); 
    MOCK_METHOD(void, handleViewSms, (Sms&), (final));
    MOCK_METHOD(void, handleSendSms, (const common::PhoneNumber&, const std::string&), (final));
};

class IUserPortMock : public IUserPort
{
public:
    IUserPortMock();
    ~IUserPortMock() override;

    MOCK_METHOD(void, showNotConnected, (), (final));
    MOCK_METHOD(void, showConnecting, (), (final));
    MOCK_METHOD(void, showConnected, (), (final));
    MOCK_METHOD(void, showPeerUserNotAvailable, (common::PhoneNumber), (override));

    MOCK_METHOD(void, acceptCallback, (IUeGui::Callback), (override));
    MOCK_METHOD(void, rejectCallback, (IUeGui::Callback), (override));
    MOCK_METHOD(void, homeCallback, (IUeGui::Callback), (override));
    MOCK_METHOD(void, doubleClickCallback, (IUeGui::Callback), (override));
    MOCK_METHOD(void, showCallRequest, (common::PhoneNumber), (override));
    MOCK_METHOD(void, showText, (const std::string& text), (override));
    MOCK_METHOD(int, getSelectedSmsIndex, (), (const, override));
    MOCK_METHOD(void, showCallDropped, (), (override));
    MOCK_METHOD(void, showCallTimeout, (), (override));

    MOCK_METHOD(void, showSmsList, (SmsDb&), (final));
    MOCK_METHOD(void, showSMS, (const Sms&), (final));
    MOCK_METHOD(void, composeSMS, (), (final));
    MOCK_METHOD(void, showNewSmsNotification, (), (override));
    MOCK_METHOD(void, noNewSmsNotification, (), (override));

    MOCK_METHOD(int, fetchScreenId, (), (override));
    MOCK_METHOD(void, setCloseGuard, (IUeGui::CloseGuard guard), (override));
    MOCK_METHOD(void, showCallAlert, (const std::string&, std::function<void()>), (override));

    MOCK_METHOD(IUeGui::IDialMode&, activateDialMode, (), (override));
    MOCK_METHOD(IUeGui::ICallMode&, activateCallMode, (), (override));
};

}
