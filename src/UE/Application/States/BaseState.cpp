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

void BaseState::handleCallMessage(common::MessageId msgId) 
{
    logger.logError("BaseState: handling call message.");
}

void BaseState::handleSMSReceive(common::PhoneNumber from, std::string text){
    logger.logError("Unexpected: handleSMSReceive from: ", from, " text: ", text);
}

void BaseState::handleSMSSent(common::PhoneNumber to){
    logger.logError("Unexpected: handleSMSSent for: ", to);
}

void BaseState::handleSMSCompose(common::PhoneNumber to, const std::string &text){
    logger.logError("Unexpected: handleMSMSCompose for: ", to, ", text: ", text);
}
}
