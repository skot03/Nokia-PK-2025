#include "ComposeSmsState.hpp"
#include "States/ConnectedState.hpp"

namespace ue {

ComposeSmsState::ComposeSmsState(Context& context)
    : ConnectedState(context)
{
    context.user.composeSMS();
}

void ComposeSmsState::handleSendSms(common::PhoneNumber to, std::string text)
{
    context.bts.sendSms(to, text);
    context.smsDb.addSentSMS(context.phoneNumber, text);
    context.setState<ConnectedState>();
}

}
