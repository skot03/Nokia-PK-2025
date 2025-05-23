#pragma once

#include "States/ConnectedState.hpp"

namespace ue {

class ViewSmsListState : public ConnectedState
{
public:
    ViewSmsListState(Context& context);
};

}