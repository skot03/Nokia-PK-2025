#include "ConnectingState.hpp"
#include "ConnectedState.hpp"
#include "NotConnectedState.hpp"

namespace ue
{

    ConnectingState::ConnectingState(Context &context)
        : BaseState(context, "ConnectingState")
    {
        context.user.showConnecting();
    }

void ConnectingState::enter()
{
    using namespace std::chrono_literals;
    context.logger.logInfo("ConnectingState::enter() – uruchamiam timer");
    context.timer.startTimer(500ms);
}

    void ConnectingState::handleAttachAccept()
    {
        context.timer.stopTimer();
        context.setState<ConnectedState>();
    }

    void ConnectingState::handleAttachReject()
    {
        context.timer.stopTimer();
        context.setState<NotConnectedState>();
    }

    void ConnectingState::handleTimeout()
    {
        context.user.showNotConnected();
        context.setState<NotConnectedState>();
    }

}
