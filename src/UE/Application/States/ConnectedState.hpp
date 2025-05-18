#pragma once

#include "BaseState.hpp"

namespace ue
{

class ConnectedState : public BaseState
{
public:
    ConnectedState(Context& context);
    void handleDisconnected() override;
    void handleCallMessage(common::MessageId msgId, common::PhoneNumber from) override;
    virtual void switchScreen();
};

}
