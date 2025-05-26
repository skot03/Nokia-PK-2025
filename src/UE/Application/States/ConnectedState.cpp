#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "ScreenManager.hpp"
#include "DialState.hpp"
#include "TalkingState.hpp"
#include "ComposeSmsState.hpp"
#include "ViewSmsListState.hpp"

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
        case ScreenManager::COMPOSE_SMS:
            context.setState<ComposeSmsState>();
            break;
        case ScreenManager::VIEW_SMS:
            context.setState<ViewSmsListState>();
            break;
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

void ConnectedState::handleReceiveSMS(common::MessageId msgId,common::PhoneNumber from,const std::string& text)
{
    
    logger.logInfo(std::string("SMS from ")+ std::to_string(from.value) + std::string(" : ") + text); 
    context.user.showNewSmsNotification();
    context.smsDb.addSMS(from, text);
}

void ConnectedState::handleSendSms(const common::PhoneNumber& to, const std::string& text)
{
    context.bts.sendSms(to, text);
    context.smsDb.addSentSMS(to, text);
    context.setState<ConnectedState>();
}

void ConnectedState::handleViewSms(Sms& sms)
{
    std::string header;

    if (sms.direction == Sms::SmsDirection::Sent) {
        header = "TO: " + to_string(sms.phoneNumber);
    } else {
        header = "FROM: " + to_string(sms.phoneNumber);
    }

    std::string fullText = header + "\n\n" + sms.text;
    if(sms.status == Sms::SmsStatus::Unread)
    {
        sms.status = Sms::SmsStatus::Read;
    }

    if(context.smsDb.getUnread()==0)
    {
        context.user.noNewSmsNotification();
    }
    else 
    {
        context.user.showNewSmsNotification();
    }
    

    context.user.showText(fullText);
}
}


