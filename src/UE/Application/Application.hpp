#pragma once

#include "Logger/PrefixedLogger.hpp"
#include "Messages/PhoneNumber.hpp"
#include "IEventsHandler.hpp"
#include "Context.hpp"
#include "Messages/IncomingMessage.hpp"
#include "States/NotConnectedState.hpp"
#include "States/ConnectedState.hpp"
#include "SMSDb.hpp"

namespace ue
{

using common::PhoneNumber;
using common::ILogger;

class Application : public IEventsHandler
{
public:
    Application(PhoneNumber phoneNumber,
                ILogger& iLogger,
                IBtsPort& bts,
                IUserPort& user,
                ITimerPort& timer);
    ~Application();

    // ITimerEventsHandler interface
    void handleTimeout() override;

    // IBtsEventsHandler interface
    void handleSib(common::BtsId btsId) override;
    void handleAttachAccept() override;
    void handleAttachReject() override;
    void handleDisconnected() override;
    void handleCallMessage(common::MessageId msgId) override;
    void handleReceiveSMS(
        common::MessageId msgId,
        common::PhoneNumber from,
        common::PhoneNumber to,
        const std::string& text
    ) override;
    void handleViewSmsList() override;
    void handleViewSms(Sms& sms) override;
    void handleSendSms(const common::PhoneNumber& from, const common::PhoneNumber& to, const std::string& text) override;
    Context& getContext() { return context; }

private:
    Context context;
    common::PrefixedLogger logger;

    friend class ApplicationTestSuite_shouldEnterConnectingState_Test;
    friend class ApplicationTestSuite_shallHandleDisconnectedInConnectedState_Test;
};

}
