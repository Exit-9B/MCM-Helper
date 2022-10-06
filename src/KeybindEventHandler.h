#pragma once

class KeybindEventHandler final : public RE::BSInputDeviceManager::Sink
{
public:
	using Event = RE::InputEvent*;

	[[nodiscard]] static KeybindEventHandler& GetInstance();
	void Register();

	RE::BSEventNotifyControl ProcessEvent(
		const Event* a_event,
		RE::BSTEventSource<Event>* a_eventSource) override;
};
