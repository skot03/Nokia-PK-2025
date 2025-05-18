#include "TalkingState.hpp"

namespace ue {

TalkingState::TalkingState(Context& context)
    : ConnectedState(context)
{
    logger.logInfo("Talking State -> Talking state started.");

    auto onHangUp = [this, &context]() {
        logger.logInfo("Talking State -> Call ended by user.");
        context.bts.sendCallDropped(context.phoneNumber);
        context.user.showConnected();
        context.setState<ConnectedState>();
    };

    context.user.rejectCallback(onHangUp);
}

void TalkingState::handleCallMessage(common::MessageId msgId, common::PhoneNumber from)
{
    if ((int)msgId == (int)common::MessageId::CallDropped) 
    {
        logger.logInfo("Call terminated by peer:" + to_string(from)); 
        context.user.showConnected();
        context.setState<ConnectedState>();
    }
}

}
