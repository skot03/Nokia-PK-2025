#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "ScreenManager.hpp"
#include "DialState.hpp"
#include "TalkingState.hpp"

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
void ConnectedState::handleCallMessage(common::MessageId msgId, common::PhoneNumber from)
{
    using common::MessageId;

    switch (msgId) {
        case MessageId::CallRequest: {
            logger.logInfo("Call request received from " + to_string(from));

            context.phoneNumber = from;
            context.user.showCallRequest(from);

            auto reject = [this, from]() {
                logger.logDebug("Call ignored from " + to_string(from));
                context.bts.sendCallDropped(from);
                context.setState<ConnectedState>();
            };

            auto accept = [this, from]() {
                logger.logDebug("Accepted call from " + to_string(from));
                context.bts.sendCallAccept(from);
                context.setState<TalkingState>();
            };

            context.user.rejectCallback(reject);
            context.user.acceptCallback(accept);
            break;
        }

        case MessageId::CallAccepted: {
            logger.logInfo("Accepted call from " + to_string(from));
            context.setState<TalkingState>();
            break;
        }

        default:
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
