#include "Config/PageContent.h"

#include "ConfigPageCache.h"
#include "Script/SkyUI.h"

void PageLayout::Draw(ScriptObjectPtr a_object) const
{
	auto& configPageCache = ConfigPageCache::GetInstance();

	configPageCache.SetGroupControls(GroupControls);

	SkyUI::Config::UnloadCustomContent();

	SkyUI::Config::SetCursorFillMode(a_object, CursorFillMode);

	for (auto& control : Controls) {
		if (control->Position >= 0) {
			SkyUI::Config::SetCursorPosition(a_object, control->Position);
		}

		auto id = control->Add(a_object);
		if (id >= 0) {
			configPageCache.AddControl(id, control);
		}
	}
}

void CustomContent::Draw(ScriptObjectPtr a_object) const
{
	SkyUI::Config::LoadCustomContent(Source, X, Y);
}
