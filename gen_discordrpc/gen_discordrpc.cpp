/*

Winamp generic plugin template code.
This code should be just the basics needed to get a plugin up and running.
You can then expand the code to build your own plugin.

Updated details compiled June 2009 by culix, based on the excellent code examples
and advice of forum members Kaboon, kichik, baafie, burek021, and bananskib.
Thanks for the help everyone!

*/

#include "stdafx.h"
#include <stdlib.h>
#include <windows.h>
#include "gen_discordrpc.h"
#include "Winamp/GEN.H"
#include "Winamp/wa_ipc.h"
#include "discord_rpc.h"

extern "C" int init(void);
extern "C" void config(void);
extern "C" void quit(void);

// this structure contains plugin information, version, name...
// GPPHDR_VER is the version of the winampGeneralPurposePlugin (GPP) structure
winampGeneralPurposePlugin plugin = { GPPHDR_VER, const_cast<char *>(PLUGIN_NAME), init, config, quit, 0, 0 };

static void updateDiscordPresence()
{
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

	wchar_t wdetails[1024];
	wsprintf(wdetails, L"%s - %s",
		artist, title
	);

	wchar_t wstate[1024];
	wsprintf(wstate, L"%s: %d:%02d / %d:%02d",
		(status == 1 ? L"Playing" : status == 3 ? L"Paused" : status == 0 ? L"Stopped" : L"Error"),
		current / 1000 / 60, current / 1000 % 60, total / 1000 / 60, total / 1000 % 60
	);

	//MessageBox(plugin.hwndParent, wstate, L"Winamp Version", MB_OK);

	size_t len;
	char details[1024];
	wcstombs_s(&len, details, sizeof details, wdetails, sizeof details);
	char state[1024];
	wcstombs_s(&len, state, sizeof state, wstate, sizeof state);

	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.state = state;
	discordPresence.details = details;
	discordPresence.largeImageKey = "winamp";
	Discord_UpdatePresence(&discordPresence);
}

extern "C" int init() {
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);

	return 0;
}

extern "C" void config() {
	updateDiscordPresence();
}

extern "C" void quit() {
	Discord_Shutdown();
}


// This is an export function called by winamp which returns this plugin info.
// We wrap the code in 'extern "C"' to ensure the export isn't mangled if used in a CPP file.
extern "C" __declspec(dllexport) winampGeneralPurposePlugin * winampGetGeneralPurposePlugin() {
	return &plugin;
}
