#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"
#include "ScreenManager.hpp"
#include "DialState.hpp"

namespace ue
{

ConnectedState::ConnectedState(Context &context)
    : BaseState(context, "ConnectedState")
{
    context.user.doubleClickCallback([this] { switchScreen();});
    context.user.showConnected();
}

void ConnectedState::handleDisconnected()
{
    logger.logInfo("Connection to BTS lost – re-attaching");
    context.user.showNotConnected();
    context.setState<NotConnectedState>();
}

    void ConnectedState::switchScreen() {
        switch (context.user.fetchScreenId()) {
            case ScreenManager::CALL_VIEW:
                context.setState<DialState>();
                break;
            case ScreenManager::NO_VIEW:
                logger.logInfo("No view!");
                break;
        }
}
}
