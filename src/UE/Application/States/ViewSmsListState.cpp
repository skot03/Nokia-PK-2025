#include "ViewSmsListState.hpp"
#include "States/ConnectedState.hpp"
#include "Context.hpp"

namespace ue {

ViewSmsListState::ViewSmsListState(Context& context)
    : ConnectedState(context)
{
    context.user.showSmsList(context.smsDb);

    context.user.acceptCallback([this, &context] {
        int index = context.user.getSelectedSmsIndex();
        const auto& messages = context.smsDb.getAllSMS();

        if (index >= 0) {
            auto& sms = messages.at(index);
            std::string fullText = "FROM: " + to_string(sms.phoneNumber) + "\n\n" + sms.text;
            context.user.showText(fullText);
            context.smsDb.markAsRead(index);
        }
    });
}

}
