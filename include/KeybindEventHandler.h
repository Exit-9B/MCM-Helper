#pragma once

class KeybindEventHandler final : public RE::BSInputDeviceManager::Sink
{
public:
	using Event = RE::InputEvent*;

	static auto GetInstance() -> KeybindEventHandler&;
	void Register();

	auto ProcessEvent(
		const Event* a_event,
		RE::BSTEventSource<Event>* a_eventSource)
		-> RE::BSEventNotifyControl override;
};