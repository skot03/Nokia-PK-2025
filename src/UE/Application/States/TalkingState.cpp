#include "UeGui/ICallMode.hpp"
#include "TalkingState.hpp"

namespace ue {

    TalkingState::TalkingState(Context& context)
    : ConnectedState(context), iCallMode(context.user.activateCallMode())
{
    logger.logInfo("Talking State -> Talking state started.");

    iCallMode.clearIncomingText();
    iCallMode.clearOutgoingText();

    auto onAccept = [this, &context]() {
        const auto text = iCallMode.getOutgoingText();
        if (!text.empty()) {
            context.bts.sendCallTalk(context.phoneNumber, text);
            iCallMode.clearOutgoingText();
            iCallMode.appendIncomingText("(Myself): " + text);
        }
    };

    auto onReject = [this, &context]() {
        logger.logInfo("TalkingState --> Notifying remote party of call termination: ", to_string(context.phoneNumber));
        context.bts.sendCallDropped(context.phoneNumber);
        context.user.showConnected();
        context.setState<ConnectedState>();

    };

    auto guard = [this,&context]() -> bool {
        context.bts.sendCallDropped(context.phoneNumber);
        return true;
    };

    context.user.homeCallback(nullptr);

    context.user.acceptCallback(onAccept);
    context.user.rejectCallback(onReject);
    context.user.setCloseGuard(guard);
}

void TalkingState::handleCallTalk(common::PhoneNumber from, const std::string& text)
{
    iCallMode.appendIncomingText("(" + to_string(from) + "): " + text);
}


void TalkingState::handleCallMessage(common::MessageId msgId, common::PhoneNumber from)
{
    logger.logInfo("Received Message: ", to_string(msgId));

    using common::MessageId;
    switch (msgId) {
        case MessageId::CallDropped:
            logger.logInfo("Call terminated by peer:" + to_string(from));
            context.user.setCloseGuard(nullptr);
            context.setState<ConnectedState>();
            context.user.showCallDropped();
            break;

        case MessageId::CallRequest:
            if (from != context.phoneNumber) {
                logger.logInfo("During a conversation - rejecting a new call from:", to_string(from));
                context.bts.sendCallDropped(from);
            }
            break;

        default:
            break;
    }
}


}
