#pragma once

#include "IEventsHandler.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "Context.hpp"
#include "Messages/IncomingMessage.hpp"

namespace ue
{

struct Context;

class BaseState : public IEventsHandler
{
public:
    BaseState(Context& context, const std::string& name);
    ~BaseState() override;

    virtual void enter() {};

    // ITimerEventsHandler interface
    void handleTimeout() override;

    // IBtsEventsHandler interface
    void handleSib(common::BtsId btsId) override;
    void handleAttachAccept() override;
    void handleAttachReject() override;
    void handleDisconnected() override;
    void handleCallMessage(common::MessageId msgId) override;
    void handleSMSReceive(common::PhoneNumber from, std::string text) override;
    void handleSMSSent(common::PhoneNumber to) override;
    void handleSMSCompose(common::PhoneNumber to, const std::string &text) override;
    
protected:
    Context& context;
    common::PrefixedLogger logger;
};

}
