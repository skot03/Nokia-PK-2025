#include "BaseState.hpp"
#include "NotConnectedState.hpp"

namespace ue
{

BaseState::BaseState(Context &context, const std::string &name)
    : context(context),
      logger(context.logger, "[" + name + "]")
{
    logger.logDebug("entry");
}

BaseState::~BaseState()
{
    logger.logDebug("exit");
}

void BaseState::handleTimeout()
{
    logger.logError("Uexpected: handleTimeout");
}

void BaseState::handleSib(common::BtsId btsId)
{
    logger.logError("Uexpected: handleSib: ", btsId);
}

void BaseState::handleAttachAccept()
{
    logger.logError("Uexpected: handleAttachAccept");
}

void BaseState::handleAttachReject()
{
    logger.logError("Uexpected: handleAttachReject");
}

void BaseState::handleDisconnected()
{
    logger.logError("handleDisconnected not implemented for this state");
}

void BaseState::handleCallReceive(common::MessageId msgId, common::PhoneNumber from)
{
    logger.logError("Processing incoming call");
}

void BaseState::handleCallMessage(common::MessageId msgId, common::PhoneNumber from)
{
    logger.logInfo("Received call message from", to_string(from), ", msgId=", (int)(msgId));
}

}
