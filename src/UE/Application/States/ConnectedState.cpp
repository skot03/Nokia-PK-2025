#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "ScreenManager.hpp"
#include "DialState.hpp"

namespace ue
{

ConnectedState::ConnectedState(Context &context)
    : BaseState(context, "ConnectedState")
{
    context.user.doubleClickCallback([this] { switchScreen();});
    context.user.showConnected();
}

void ConnectedState::handleDisconnected()
{
    logger.logInfo("Connection to BTS lost – re-attaching");
    context.user.showNotConnected();
    context.setState<NotConnectedState>();
}

    void ConnectedState::switchScreen() {
        switch (context.user.fetchScreenId()) {
            case ScreenManager::CALL_VIEW:
                context.setState<DialState>();
                break;
            case ScreenManager::NO_VIEW:
                logger.logInfo("No view!");
                break;
        }
}

void ConnectedState::handleReceiveSMS(common::MessageId msgId,
    common::PhoneNumber from,
    const std::string& text) {
    std::string log = std::string("SMS From ")
    + std::to_string(from.value) + std::string(", content: ") + text;

    logger.logInfo(log);
    context.smsDb.addSMS(from, text); 

}

void ConnectedState::handleViewSmsList()
{
    logger.logInfo("View SMS list");
    context.user.showSmsList(context.smsDb);
}

void ConnectedState::handleViewSms(Sms& sms)
{
    //TODO
}

void ConnectedState::handleSendSms(const common::PhoneNumber& from, const std::string& text)
{
    context.logger.logInfo("Send SMS to ", (int) from.value, " : ", text);

    context.smsDb.addSentSMS(from, text);
}

}
