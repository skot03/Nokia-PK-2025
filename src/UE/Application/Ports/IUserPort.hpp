#pragma once
#include "SMSDb.hpp"
#include <IUeGui.hpp>

namespace ue
{

class IUserEventsHandler
{
public:
    virtual ~IUserEventsHandler() = default;
    virtual void handleViewSmsList() = 0;
    virtual void handleViewSms(Sms& sms) = 0;
    virtual void handleSendSms(const common::PhoneNumber& from, const std::string& text) = 0;
};

class IUserPort
{
public:
    virtual ~IUserPort() = default;

    virtual void showNotConnected() = 0;
    virtual void showConnecting() = 0;
    virtual void showConnected() = 0;
    virtual void showPeerUserNotAvailable(common::PhoneNumber number) = 0;
    virtual void showSmsList(SmsDb& smsdb) = 0;
    virtual void showSMS(const Sms& sms) = 0;
    virtual void showText(const std::string& text) = 0;
    virtual int getSelectedSmsIndex() const = 0;
    virtual void composeSMS() = 0; 
    virtual void acceptCallback(IUeGui::Callback acceptCallback) = 0;
    virtual void rejectCallback(IUeGui::Callback rejectCallback) = 0;
    virtual void doubleClickCallback(IUeGui::Callback doubleClickCallback) = 0;
    virtual void homeCallback(IUeGui::Callback homeCallback) = 0;
    virtual int fetchScreenId() = 0;

    virtual void showCallRequest(common::PhoneNumber) = 0;
    
    virtual IUeGui::IDialMode &activateDialMode() = 0;
};

}
