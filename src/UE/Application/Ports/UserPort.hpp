#pragma once

#include "IUserPort.hpp"
#include "Logger/PrefixedLogger.hpp"
#include "IUeGui.hpp"
#include "Messages/PhoneNumber.hpp"
#include "ScreenManager.hpp"



namespace ue
{

class UserPort : public IUserPort
{
public:
    UserPort(common::ILogger& logger, IUeGui& gui, common::PhoneNumber phoneNumber);
    void start(IUserEventsHandler& handler);
    void stop();

    void showCallRequest(common::PhoneNumber) override;
    void showNotConnected() override;
    void showConnecting() override;
    void showConnected() override;
    void showPeerUserNotAvailable(common::PhoneNumber number) override;

    virtual void acceptCallback(IUeGui::Callback acceptCallback) override;
    virtual void rejectCallback(IUeGui::Callback rejectCallback) override;
    
    virtual void homeCallback(IUeGui::Callback homeCallback) override;
    virtual int fetchScreenId() override;
    virtual void doubleClickCallback(IUeGui::Callback doubleClickCallback) override;

    IUeGui::IListViewMode& getMenuRef();

    virtual IUeGui::IDialMode &activateDialMode() override;


private:
    common::PrefixedLogger logger;
    IUeGui& gui;
    IUeGui::Callback callback;
    common::PhoneNumber phoneNumber;
    IUserEventsHandler* handler = nullptr;
    int currentScreen = ScreenManager::NO_VIEW;


    void selectScreen(IUeGui::IListViewMode& menu);
};

}
