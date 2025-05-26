#pragma once

#include "ITimerPort.hpp"
#include "Logger/PrefixedLogger.hpp"
#include <thread>
#include <atomic>

using namespace std;

namespace ue
{

    class TimerPort : public ITimerPort
    {
    public:
        TimerPort(common::ILogger &logger);

        void start(ITimerEventsHandler &handler);
        void stop();

        // ITimerPort interface
        void startTimer(Duration duration) override;
        void stopTimer() override;

        ~TimerPort();

    private:
        common::PrefixedLogger logger;
        ITimerEventsHandler *handler = nullptr;

        std::atomic_bool isTimerActive;

        void runTimer(Duration duration);
       };

}