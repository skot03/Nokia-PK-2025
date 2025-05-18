#include "UserPort.hpp"
#include "UeGui/IListViewMode.hpp"
#include "UeGui/ISmsComposeMode.hpp"

namespace ue
{

UserPort::UserPort(common::ILogger &logger, IUeGui &gui, common::PhoneNumber phoneNumber)
    : logger(logger, "[USER-PORT]"),
      gui(gui),
      phoneNumber(phoneNumber)
{}

void UserPort::start(IUserEventsHandler &handler)
{
    this->handler = &handler;
    gui.setTitle("Nokia " + to_string(phoneNumber));
}

void UserPort::stop()
{
    handler = nullptr;
}

void UserPort::showNotConnected()
{
    gui.showNotConnected();
}

void UserPort::showConnecting()
{
    gui.showConnecting();
}

void UserPort::showConnected()
{
    IUeGui::IListViewMode& menu = gui.setListViewMode();
    menu.clearSelectionList();

    menu.addSelectionListItem("Compose SMS", "");
    menu.addSelectionListItem("View SMS", "");
    menu.addSelectionListItem("Call to someone", "");

    gui.setAcceptCallback([this, &menu]{ selectScreen(menu);});
}

void UserPort::composeSMS()
{
    IUeGui::ISmsComposeMode& composeMode = gui.setSmsComposeMode();
    composeMode.clearSmsText();

    gui.setAcceptCallback([this, &composeMode]() {
        auto to = composeMode.getPhoneNumber();
        auto text = composeMode.getSmsText();

        if (to.isValid() && !text.empty())
        {
            handler->handleSendSms(phoneNumber, text);
        }
        else
        {
            logger.logError("Invalid recipient or empty message");
        }
    });

    gui.setRejectCallback([this]() {
        showConnected();
    });
}

void UserPort::showSMS(Sms& sms) {
   //TODO
}
  
void UserPort::showSmsList(SmsDb& smsdb)
{
    IUeGui::IListViewMode& menu = gui.setListViewMode();
    menu.clearSelectionList();

    for (Sms& sms : smsdb)
    {
        std::string itemText = "From: " + std::to_string(sms.phoneNumber.value);
        menu.addSelectionListItem(itemText, "");
    }

    gui.setAcceptCallback([this, &menu, &smsdb]() {
        auto selected = menu.getCurrentItemIndex();
        Sms& selectedSms = *(smsdb.begin() + selected.second);
        handler->handleViewSms(selectedSms);
    });

    gui.setRejectCallback([this]() {
        showConnected();
    });
}

void UserPort::showPeerUserNotAvailable(common::PhoneNumber number) {
    gui.showPeerUserNotAvailable(number);

    const auto handler = [&] {
        gui.setListViewMode();
    };

    gui.setAcceptCallback(handler);
    gui.setRejectCallback(handler);
    gui.setHomeCallback(handler);
}

void UserPort::acceptCallback(IUeGui::Callback acceptCallback) {
    gui.setAcceptCallback(acceptCallback);
}

void UserPort::rejectCallback(IUeGui::Callback rejectCallback) {
    gui.setRejectCallback(rejectCallback);
}

void UserPort::doubleClickCallback(IUeGui::Callback doubleClickCallback) {
    gui.setDoubleClickCallback(doubleClickCallback);
}

void UserPort::homeCallback(IUeGui::Callback homeCallback) {
    gui.setHomeCallback(homeCallback);
}

void UserPort::selectScreen(IUeGui::IListViewMode& menu) {
    IUeGui::IListViewMode::OptionalSelection pair = menu.getCurrentItemIndex();
    currentScreen = pair.first ? pair.second : ScreenManager::NO_VIEW;
}

int UserPort::fetchScreenId() {
    return currentScreen;
}

IUeGui::IDialMode &UserPort::activateDialMode() {
    IUeGui::IDialMode &mode = gui.setDialMode();

    return mode;
}



}
