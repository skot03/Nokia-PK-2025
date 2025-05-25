#include "ComposeSmsState.hpp"
#include "States/ConnectedState.hpp"

namespace ue {

ComposeSmsState::ComposeSmsState(Context& context)
    : ConnectedState(context)
{
    context.user.composeSMS();
}

}
