#pragma once

#include "Messages/BtsId.hpp"

namespace ue
{

class IBtsEventsHandler
{
public:
    virtual ~IBtsEventsHandler() = default;

    virtual void handleSib(common::BtsId) = 0;
    virtual void handleAttachAccept() = 0;
    virtual void handleAttachReject() = 0;
};

class IBtsPort
{
public:
    virtual ~IBtsPort() = default;

    virtual void start(IBtsEventsHandler& handler) = 0;
    virtual void sendAttachRequest(common::BtsId) = 0;
};

}
