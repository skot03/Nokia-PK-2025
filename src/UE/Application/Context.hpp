#pragma once

#include "IEventsHandler.hpp"
#include "Logger/ILogger.hpp"
#include <memory>
#include "SMSDb.hpp"
namespace ue
{

class BaseState;

struct Context
{
    common::ILogger& logger;
    IBtsPort& bts;
    IUserPort& user;
    ITimerPort& timer;
    //std::unique_ptr<IEventsHandler> state{};
    std::unique_ptr<BaseState> state{};
    SmsDb smsDb;
    template <typename State, typename ...Arg>
    void setState(Arg&& ...arg)
    {
        state = std::make_unique<State>(*this, std::forward<Arg>(arg)...);
        state->enter();
    }
};

}
