#include "ViewSmsListState.hpp"
#include "States/ConnectedState.hpp"
#include "Context.hpp"

namespace ue {

ViewSmsListState::ViewSmsListState(Context& context)
    : ConnectedState(context)
{
    context.user.showSmsList(context.smsDb);
}

}
