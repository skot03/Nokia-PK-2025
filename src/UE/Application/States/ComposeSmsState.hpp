#pragma once

#include "States/ConnectedState.hpp"

namespace ue {

class ComposeSmsState : public ConnectedState
{
public:
    ComposeSmsState(Context& context);

    void handleSendSms(common::PhoneNumber to, std::string text);
};

}
