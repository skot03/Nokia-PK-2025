#pragma once

#include "Messages/BtsId.hpp"
#include "Messages/PhoneNumber.hpp"
#include "Messages/IncomingMessage.hpp"

namespace ue
{

class IBtsEventsHandler
{
public:
    virtual ~IBtsEventsHandler() = default;

    virtual void handleSib(common::BtsId) = 0;
    virtual void handleAttachAccept() = 0;
    virtual void handleAttachReject() = 0;
    virtual void handleDisconnected() = 0;
    virtual void handleCallMessage(common::MessageId msgId) = 0;
    virtual void handleReceiveSMS(common::MessageId msgId,
        common::PhoneNumber from,
        common::PhoneNumber to,
        const std::string& text) = 0;
};

class IBtsPort
{
public:
    virtual ~IBtsPort() = default;
    virtual void sendSms(common::PhoneNumber to, const std::string &text) = 0;
    virtual void start(IBtsEventsHandler& handler) = 0;
    virtual void sendAttachRequest(common::BtsId) = 0;
    virtual void sendCallRequest(common::PhoneNumber) = 0;

};

}
