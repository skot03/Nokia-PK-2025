#include "UeGui/ICallMode.hpp"
#include "DialState.hpp"
#include "TalkingState.hpp"

using namespace std::chrono_literals;

namespace ue {

    DialState::DialState(Context &context) : ConnectedState(context), iDialMode(context.user.activateDialMode())
    {
        context.user.homeCallback([this]{ this->context.user.showConnected(); });
        context.user.acceptCallback([this]{ sendCallRequest(); });
        context.user.rejectCallback([this] { handleReject(); });
    }
    

    void DialState::sendCallRequest() {
        logger.logDebug("DialState -> making request to: ", iDialMode.getPhoneNumber());

        context.bts.sendCallRequest(iDialMode.getPhoneNumber());
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
            case common::MessageId::CallRequest: {
                logger.logInfo("Received CallRequest while dialing – switching to incoming call");
    
                context.timer.stopTimer(); 
                context.phoneNumber = from;
                context.user.showCallRequest(from);
    
                auto reject = [this, from]() {
                    logger.logDebug("Call ignored from " + to_string(from));
                    context.timer.stopTimer();
                    context.bts.sendCallDropped(from);
                    context.setState<ConnectedState>();
        
                };
    
                auto accept = [this, from]() {
                    logger.logDebug("Accepted call from " + to_string(from));
                    context.timer.stopTimer();
                    context.bts.sendCallAccept(from);
                    context.setState<TalkingState>();

                };
    
                context.user.rejectCallback(reject);
                context.user.acceptCallback(accept);
    
                context.timer.startTimer(std::chrono::seconds(30));
                break;
            }
        }
    }


    void DialState::handleReject() {
        logger.logDebug("DialState -> user cancelled outgoing call");
        context.timer.stopTimer();
        context.bts.sendCallDropped(iDialMode.getPhoneNumber()); 
        context.setState<ConnectedState>();
        context.user.showConnected(); 
    }

    void DialState::handleTimeout() {
        logger.logInfo("Incoming call timeout – dropping call");

        context.bts.sendCallDropped(context.phoneNumber);

        context.setState<ConnectedState>();
    }


}