#pragma once

#include "ConnectedState.hpp"
#include "UeGui/IDialMode.hpp"
#include "Messages/IncomingMessage.hpp"


namespace ue
{
    class DialState : public ConnectedState
    {
        public:
            DialState(Context& context);
            void sendCallRequest();
            void handleTimeout() override;
            void handleCallMessage(common::MessageId msgId, common::PhoneNumber from) override;

        private:
            IUeGui::IDialMode& iDialMode;
    };
}