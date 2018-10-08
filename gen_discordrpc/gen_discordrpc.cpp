/*

Winamp generic plugin template code.
This code should be just the basics needed to get a plugin up and running.
You can then expand the code to build your own plugin.

Updated details compiled June 2009 by culix, based on the excellent code examples
and advice of forum members Kaboon, kichik, baafie, burek021, and bananskib.
Thanks for the help everyone!

*/

#include "stdafx.h"
#include <windows.h>
#include "gen_discordrpc.h"
#include "Winamp/GEN.H"


extern "C" int init(void);
extern "C" void config(void);
extern "C" void quit(void);

// this structure contains plugin information, version, name...
// GPPHDR_VER is the version of the winampGeneralPurposePlugin (GPP) structure
winampGeneralPurposePlugin plugin = { GPPHDR_VER, const_cast<char *>(PLUGIN_NAME), init, config, quit, 0, 0 };

extern "C" int init() {
	//A basic messagebox that tells you the 'init' event has been triggered.
	//If everything works you should see this message when you start Winamp once your plugin has been installed.
	//You can change this later to do whatever you want (including nothing)
	MessageBox(plugin.hwndParent, L"Init event triggered for gen_myplugin. Plugin installed successfully!", L"", MB_OK);
	return 0;
}

extern "C" void config() {
	//A basic messagebox that tells you the 'config' event has been triggered.
	//You can change this later to do whatever you want (including nothing)
	//MessageBox(plugin.hwndParent, L"Config event triggered for gen_myplugin.", L"", MB_OK);
}

extern "C" void quit() {
	//A basic messagebox that tells you the 'quit' event has been triggered.
	//If everything works you should see this message when you quit Winamp once your plugin has been installed.
	//You can change this later to do whatever you want (including nothing)
	//MessageBox(0, L"Quit event triggered for gen_myplugin.", L"", MB_OK);
}


// This is an export function called by winamp which returns this plugin info.
// We wrap the code in 'extern "C"' to ensure the export isn't mangled if used in a CPP file.
extern "C" __declspec(dllexport) winampGeneralPurposePlugin * winampGetGeneralPurposePlugin() {
	return &plugin;
}
