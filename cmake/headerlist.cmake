set(HEADERS ${HEADERS}
    include/PCH.h

    include/ColorUtil.h
    include/FormUtil.h
    include/InputMap.h
    include/Encoding.h
    include/Translation.h

    include/SettingStore.h
    include/ConfigStore.h
    include/ConfigPageCache.h
    include/KeybindManager.h
    include/KeybindEventHandler.h

    include/Script/ScriptObject.h
    include/Script/SkyUI.h

    include/Config/Config.h
    include/Config/PageContent.h
    include/Config/Control.h
    include/Config/ValueSource.h
    include/Config/TextSource.h
    include/Config/Action.h
    include/Config/GroupConditionTree.h

    include/Papyrus/RegisterFuncs.h
    include/Papyrus/MCM.h
    include/Papyrus/MCM_ConfigBase.h

    include/Json/ReaderHandler.h
    include/Json/IHandler.h
    include/Json/IHandler.inl
    include/Json/ConfigHandler.h
    include/Json/PluginRequirementsHandler.h
    include/Json/ContentHandler.h
    include/Json/CustomContentHandler.h
    include/Json/PagesHandler.h
    include/Json/ValueOptionsHandler.h
    include/Json/ActionHandler.h
    include/Json/ParamsHandler.h
    include/Json/GroupConditionHandler.h
    include/Json/KeybindsHandler.h
    include/Json/KeybindsArrayHandler.h
    include/Json/UserKeybindsHandler.h
    include/Json/UserKeybindsArrayHandler.h
)
