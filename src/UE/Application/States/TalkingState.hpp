#pragma once

#include "ConnectedState.hpp"

namespace ue
{
    class TalkingState : public ConnectedState
    {
        IUeGui::ICallMode& iCallMode;
    public:
        TalkingState(Context& context);
        void handleCallTalk(common::PhoneNumber from, const std::string& text) override;
        void handleCallMessage(common::MessageId msgId, common::PhoneNumber from) override;
    };
}
