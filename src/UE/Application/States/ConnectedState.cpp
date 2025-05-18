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
    if ((int)msgId == (int)common::MessageId::CallRequest) 
    {
        logger.logInfo("Call request received from " + to_string(from)); 

        context.user.showCallRequest(from);
        context.phoneNumber = from;

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
    }
}
}
