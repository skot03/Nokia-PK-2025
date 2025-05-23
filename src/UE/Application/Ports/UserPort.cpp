#include "UserPort.hpp"
#include "UeGui/ITextMode.hpp"
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
            handler->handleSendSms(to, text);
        }
    });


    gui.setRejectCallback([this]() {
        showConnected();
    });
}

void UserPort::showSMS(const Sms& sms) {
   gui.setViewTextMode().setText("FROM: " + to_string(sms.phoneNumber) + "\n\n" + sms.text);
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

void UserPort::showText(const std::string& text)
{
    gui.setViewTextMode().setText(text);
}

int UserPort::getSelectedSmsIndex() const {
    IUeGui::IListViewMode& menu = gui.setListViewMode();
    IUeGui::IListViewMode::OptionalSelection sel = menu.getCurrentItemIndex();
    return sel.first ? sel.second : -1;
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

void UserPort::showCallRequest(common::PhoneNumber number)
{
    auto &alertMode = gui.setAlertMode();
    alertMode.setText("Phone call by: " + to_string(number));
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

void UserPort::showCallAlert(const std::string& message, std::function<void()> callback)
{
    auto& alertMode = gui.setAlertMode();
    alertMode.setText(message);

    gui.setAcceptCallback(callback);
    gui.setRejectCallback(callback);
    gui.setHomeCallback(callback);
}

void UserPort::showCallDropped() 
{
    showCallAlert("Call dropped.", [this] {
        showConnected();
    });
}

void UserPort::showCallTimeout()
{
    showCallAlert("Call timeout.", [this] {
        gui.showConnected();
    });
}

void UserPort::setCloseGuard(IUeGui::CloseGuard guard)
{
    gui.setCloseGuard(guard);
}

IUeGui::ICallMode &UserPort::activateCallMode() {
    IUeGui::ICallMode &mode = gui.setCallMode();

    return mode;
}

IUeGui::IDialMode &UserPort::activateDialMode() {
    IUeGui::IDialMode &mode = gui.setDialMode();

    return mode;
}



}
