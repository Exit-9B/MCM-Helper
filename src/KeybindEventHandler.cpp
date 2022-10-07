#include "KeybindEventHandler.h"
#include "KeybindManager.h"

KeybindEventHandler& KeybindEventHandler::GetInstance()
{
	static KeybindEventHandler instance{};
	return instance;
}

void KeybindEventHandler::Register()
{
	const auto inputDeviceManager = RE::BSInputDeviceManager::GetSingleton();
	if (inputDeviceManager) {
		inputDeviceManager->AddEventSink(this);
	}
}

RE::BSEventNotifyControl KeybindEventHandler::ProcessEvent(
	const Event* a_event,
	[[maybe_unused]] RE::BSTEventSource<Event>* a_eventSource)
{
	const auto ui = RE::UI::GetSingleton();
	if (!ui || ui->GameIsPaused())
		return RE::BSEventNotifyControl::kContinue;

	const auto inputEvent = a_event ? *a_event : nullptr;
	const auto buttonEvent = inputEvent ? inputEvent->AsButtonEvent() : nullptr;

	if (buttonEvent) {
		const auto idCode = buttonEvent->GetIDCode();

		if (idCode == 0) {
			return RE::BSEventNotifyControl::kContinue;
		}

		KeybindManager::GetInstance().ProcessButtonEvent(buttonEvent);
	}

	return RE::BSEventNotifyControl::kContinue;
}
