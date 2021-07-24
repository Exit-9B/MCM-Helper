set(sources ${sources}
    src/main.cpp
    src/PCH.cpp

    src/Utils.cpp
    src/SettingStore.cpp
    src/ConfigStore.cpp
    src/ConfigPageCache.cpp
    src/KeybindManager.cpp
    src/KeybindEventHandler.cpp

    src/Script/ScriptObject.cpp
    src/Script/SkyUI.cpp

    src/Config/Config.cpp
    src/Config/PageContent.cpp
    src/Config/Control.cpp
    src/Config/ValueSource.cpp
    src/Config/Action.cpp
    src/Config/GroupConditionTree.cpp

    src/Papyrus/RegisterFuncs.cpp
    src/Papyrus/MCM.cpp
    src/Papyrus/MCM_ConfigBase.cpp

    src/Json/ReaderHandler.cpp
    src/Json/IHandler.cpp
    src/Json/ConfigHandler.cpp
    src/Json/PluginRequirementsHandler.cpp
    src/Json/ContentHandler.cpp
    src/Json/CustomContentHandler.cpp
    src/Json/PagesHandler.cpp
    src/Json/ValueOptionsHandler.cpp
    src/Json/ActionHandler.cpp
    src/Json/ParamsHandler.cpp
    src/Json/GroupConditionHandler.cpp
    src/Json/KeybindsHandler.cpp
    src/Json/KeybindsArrayHandler.cpp
)
