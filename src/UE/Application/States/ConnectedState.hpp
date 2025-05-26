#pragma once

#include "BaseState.hpp"

namespace ue
{

class ConnectedState : public BaseState
{
public:
    ConnectedState(Context& context);
    void handleDisconnected() override;
    void handleCallMessage(common::MessageId msgId, common::PhoneNumber from) override;
    void handleReceiveSMS(common::MessageId msgId,common::PhoneNumber from,const std::string& text) override;
    void handleSendSms(const common::PhoneNumber& to, const std::string& text) override;
    virtual void switchScreen();
    void handleViewSms(Sms& sms) override;
    void handleTimeout() override;
};

}
