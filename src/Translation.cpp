#include "Translation.h"

auto Translation::ScaleformTranslate(const std::string& a_key) -> std::string
{
	if (!a_key.starts_with('$')) {
		return a_key;
	}

	const auto scaleformManager = RE::BSScaleformManager::GetSingleton();
	const auto loader = scaleformManager ? scaleformManager->loader : nullptr;
	const auto translator = loader
		? loader->GetState<RE::GFxTranslator>(RE::GFxState::StateType::kTranslator)
		: nullptr;

	if (!translator) {
		logger::warn("Failed to get Scaleform translator"sv);
		return a_key;
	}

	// Count braces to find what to replace
	std::string key = a_key;
	std::vector<std::string> nested;

	auto nestOpen = a_key.find_first_of('{');
	if (nestOpen != std::string::npos) {
		auto nestLevel = 1;
		auto tokenStart = nestOpen + 1;
		for (auto i = tokenStart; i < key.length(); i++) {
			if (key[i] == '{') {
				nestLevel++;
				if (nestLevel == 1) {
					tokenStart = i + 1;
				}
			}
			else if (key[i] == '}') {
				nestLevel--;
				if (nestLevel == 0) {
					// Recursion
					auto nestedKey = key.substr(tokenStart, i - tokenStart);
					nested.push_back(ScaleformTranslate(nestedKey));
					key = key.substr(0, tokenStart) + key.substr(i, key.length() - i);
					i = tokenStart + 1;
				}
			}
		}
	}

	// Lookup translation
	std::wstring key_utf16 = util::utf8_to_utf16(key).value_or(L""s);
	RE::GFxWStringBuffer result;

	RE::GFxTranslator::TranslateInfo translateInfo;
	translateInfo.key = key_utf16.c_str();
	translateInfo.result = std::addressof(result);

	translator->Translate(std::addressof(translateInfo));

	std::string result_utf8 = util::utf16_to_utf8(result.c_str()).value_or(""s);

	// Replace tokens with nested translations from right to left
	auto pos = result_utf8.rfind("{}"s);
	while (pos != std::string::npos && !nested.empty()) {
		result_utf8 = result_utf8.replace(pos, 2, nested.back());
		nested.pop_back();
		pos = result_utf8.rfind("{}"s, pos);
	}

	return result_utf8;
}
