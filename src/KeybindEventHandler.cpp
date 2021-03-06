#include "KeybindEventHandler.h"
#include "KeybindManager.h"

auto KeybindEventHandler::GetInstance() -> KeybindEventHandler&
{
	static KeybindEventHandler instance;
	return instance;
}

void KeybindEventHandler::Register()
{
	const auto inputDeviceManager = RE::BSInputDeviceManager::GetSingleton();
	if (inputDeviceManager) {
		inputDeviceManager->AddEventSink(this);
	}
}

auto KeybindEventHandler::ProcessEvent(
	const Event* a_event,
	[[maybe_unused]] RE::BSTEventSource<Event>* a_eventSource) -> RE::BSEventNotifyControl
{
	const auto ui = RE::UI::GetSingleton();
	if (!ui || ui->GameIsPaused())
		return RE::BSEventNotifyControl::kContinue;

	auto inputEvent = a_event ? *a_event : nullptr;
	auto buttonEvent = inputEvent ? inputEvent->AsButtonEvent() : nullptr;

	if (buttonEvent) {
		auto idCode = buttonEvent->GetIDCode();
		auto deviceType = buttonEvent->device;

		if (idCode == 0) {
			return RE::BSEventNotifyControl::kContinue;
		}

		KeybindManager::GetInstance().ProcessButtonEvent(buttonEvent);
	}

	return RE::BSEventNotifyControl::kContinue;
}
