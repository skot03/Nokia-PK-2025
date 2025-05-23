#include "UeGui/ICallMode.hpp"
#include "DialState.hpp"
#include "TalkingState.hpp"

using namespace std::chrono_literals;

namespace ue {

    DialState::DialState(Context &context) : ConnectedState(context), iDialMode(context.user.activateDialMode())
    {
        context.user.homeCallback([this]{ this->context.user.showConnected(); });
        context.user.acceptCallback([this]{ sendCallRequest(); });
    }

    void DialState::sendCallRequest() {
        logger.logDebug("DialState -> making request to: ", iDialMode.getPhoneNumber());

        context.bts.sendCallRequest(iDialMode.getPhoneNumber());
        context.timer.startTimer(1000ms);
    }

    void DialState::handleCallMessage(common::MessageId msgId, common::PhoneNumber from) {
        context.phoneNumber = from;

        switch (msgId) {
            case common::MessageId::CallAccepted:
                context.timer.stopTimer();
                context.setState<TalkingState>();
            break;
            case common::MessageId::CallDropped:
                context.timer.stopTimer();
                context.setState<ConnectedState>();
                context.user.showCallDropped();
            break;
            case common::MessageId::UnknownRecipient:
                context.timer.stopTimer();
                context.setState<ConnectedState>();
                context.user.showPeerUserNotAvailable(iDialMode.getPhoneNumber());
            break;
        }
    }

    void DialState::handleReject(common::PhoneNumber from) {
        context.timer.stopTimer();
        context.bts.sendCallDropped(from);
        context.setState<ConnectedState>();
    }

    void DialState::handleTimeout() {
        context.timer.stopTimer();
        context.setState<ConnectedState>();
        context.user.showCallTimeout();
    }


}