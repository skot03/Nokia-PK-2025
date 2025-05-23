#include "Application.hpp"
#include "States/NotConnectedState.hpp"
#include "States/ConnectedState.hpp"

namespace ue
{

Application::Application(common::PhoneNumber phoneNumber,
                         common::ILogger &iLogger,
                         IBtsPort &bts,
                         IUserPort &user,
                         ITimerPort &timer)
    : context{phoneNumber,iLogger, bts, user, timer},
      logger(iLogger, "[APP] ")
{
    logger.logInfo("Started");
    context.setState<NotConnectedState>();
    context.bts.start(*this);
}

Application::~Application()
{
    logger.logInfo("Stopped");
}

void Application::handleTimeout()
{
    context.state->handleTimeout();
}

void Application::handleSib(common::BtsId btsId)
{
    context.state->handleSib(btsId);
}

void Application::handleAttachAccept()
{
    context.state->handleAttachAccept();
}

void Application::handleAttachReject()
{
    context.state->handleAttachReject();
}

void Application::handleDisconnected()
{
    context.state->handleDisconnected();
    logger.logInfo("Application received disconnect event");
}

void Application::handleCallReceive(common::MessageId msgId, common::PhoneNumber from)
{
    context.state->handleCallReceive(msgId, from);

}

void Application::handleCallMessage(common::MessageId msgId, common::PhoneNumber from)
{
    context.state->handleCallMessage(msgId, from);
}

void Application::handleViewSmsList()
{
    context.state->handleViewSmsList();
}

void Application::handleViewSms(Sms& sms)
{
    context.state->handleViewSms(sms);
}

void Application::handleSendSms(const common::PhoneNumber& to, const std::string& text)
{
    context.state->handleSendSms(to, text);
}

void Application::handleReceiveSMS(common::MessageId msgId,common::PhoneNumber from,const std::string& text)
{
    context.state->handleReceiveSMS(msgId, from, text);
}

void Application::handleCallTalk(common::PhoneNumber from, const std::string& text)
{
    context.state->handleCallTalk(from, text);
}


}
