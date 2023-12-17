scriptname SKI_ConfigBase extends SKI_QuestBase hidden

;##################################################################################################
; API Version:		4
;##################################################################################################
;
; Base script for custom config menus.
;
; This file contains the public interface of SKI_ConfigBase so you're able to extend it.
;
; DO NOT MODIFY THIS SCRIPT!
; DO NOT RECOMPILE THIS SCRIPT!
;
;##################################################################################################

; PROPERTIES --------------------------------------------------------------------------------------

string property		ModName auto

string[] property	Pages auto

string property		CurrentPage
	string function get()
		Guard()
		return  ""
	endFunction
endProperty


; EVENTS ------------------------------------------------------------------------------------------

event OnConfigInit()
	{Called when this config menu is initialized}
	Guard()
endEvent

event OnConfigOpen()
	{Called when this config menu is opened}
	Guard()
endEvent

event OnConfigClose()
	{Called when this config menu is closed}
	Guard()
endEvent

event OnVersionUpdate(int aVersion)
	{Called when aVersion update of this script has been detected}
	Guard()
endEvent


; FUNCTIONS ---------------------------------------------------------------------------------------

int function GetVersion()
	{Returns version of this script. Override if necessary}
	Guard()
endFunction

function ForcePageReset()
	{Forces a full reset of the current page}
	Guard()
endFunction

function SetTitleText(string a_text)
	{Sets the title text of the control panel}
	Guard()
endFunction

bool function ShowMessage(string a_message, bool a_withCancel = true, string a_acceptLabel = "$Accept", string a_cancelLabel = "$Cancel")
	{Shows a message dialog and waits until the user has closed it}
	Guard()
endFunction

; -------------------------------------------------------------------------------------------------

function Guard()
	Debug.MessageBox("SKI_ConfigBase: Don't recompile this script!")
endFunction

