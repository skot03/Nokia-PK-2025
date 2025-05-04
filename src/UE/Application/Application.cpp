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
    : context{iLogger, bts, user, timer},
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

void Application::handleCallMessage(common::MessageId msgId) 
{
    context.state->handleCallMessage(msgId);
}

void Application::handleSMSReceive(common::PhoneNumber from, std::string text)
{
    logger.logInfo("SMS received, sender: ", from);
    context.state->handleSMSReceive(from, text);
}
void Application::handleSMSSent(common::PhoneNumber to)
{
    logger.logInfo("Handling SMS send for: ", to);
    if (context.state)
        context.state->handleSMSSent(to);
}

void Application::handleSMSCompose(common::PhoneNumber to, const std::string &text){
    context.smsDb.addSMS(to, text);
    context.bts.sendSMS(to, text);
    context.setState<ConnectedState>();
}

}
