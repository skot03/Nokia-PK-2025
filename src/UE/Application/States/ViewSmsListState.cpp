#include "ViewSmsListState.hpp"
#include "States/ConnectedState.hpp"

namespace ue {

ViewSmsListState::ViewSmsListState(Context& context)
    : ConnectedState(context)
{
    context.user.showSmsList(context.smsDb);
}

}