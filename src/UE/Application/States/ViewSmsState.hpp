#pragma once

#include "States/ConnectedState.hpp"

namespace ue {

class ViewSmsState : public ConnectedState {
public:
    ViewSmsState(Context& context, int index);
    void enter() override;

private:
    int smsIndex;
};

}

