#include "States/ViewSmsState.hpp"
#include "Context.hpp"

namespace ue {

ViewSmsState::ViewSmsState(Context& context, int index)
    : ConnectedState(context), smsIndex(index)
{}

void ViewSmsState::enter()
{
    auto& sms = context.smsDb.getAllSMS().at(smsIndex);
    context.user.showSMS(sms);
}

}

