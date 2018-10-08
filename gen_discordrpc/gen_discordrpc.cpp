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
#include "Winamp/wa_ipc.h"

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
	//MessageBox(plugin.hwndParent, L"Init event triggered for gen_myplugin. Plugin installed successfully!", L"", MB_OK);
	return 0;
}

extern "C" void config() {
	wchar_t artist[512];
	wchar_t title[512];

	extendedFileInfoStructW efs;
	efs.filename = (wchar_t *)SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_GET_PLAYING_FILENAME);
	efs.metadata = L"artist";
	efs.ret = artist;
	efs.retlen = 512;
	SendMessage(plugin.hwndParent, WM_WA_IPC, (WPARAM)&efs, IPC_GET_EXTENDED_FILE_INFOW);
	efs.metadata = L"title";
	efs.ret = title;
	SendMessage(plugin.hwndParent, WM_WA_IPC, (WPARAM)&efs, IPC_GET_EXTENDED_FILE_INFOW);

	int current = SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_GETOUTPUTTIME);
	if (current == -1) {
		current = 0;
	}

	int total = SendMessage(plugin.hwndParent, WM_WA_IPC, 2, IPC_GETOUTPUTTIME);

	int status = SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_ISPLAYING);

	wchar_t msg[1024];
	wsprintf(msg, L"%s - %s\n"
		L"%s: %d:%02d / %d:%02d",
		artist, title,
		(status == 1 ? L"Playing" : status == 3 ? L"Paused" : status == 0 ? L"Stopped" : L"Error"),
		current / 1000 / 60, current / 1000 % 60, total / 1000 / 60, total / 1000 % 60
	);

	MessageBox(plugin.hwndParent, msg, L"Winamp Version", MB_OK);
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
