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
#include <thread>
#include <chrono>
#include <windows.h>
#include <strsafe.h>
#include "gen_discordrpc.h"
#include "Winamp/GEN.H"
#include "Winamp/wa_ipc.h"
#include "discord_rpc.h"
#include "resource.h"

extern "C" int init(void);
extern "C" void config(void);
extern "C" void quit(void);

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#pragma warning(disable: 4047)
HINSTANCE hInstance = (HINSTANCE)&__ImageBase;
#pragma warning(default: 4047)

// this structure contains plugin information, version, name...
// GPPHDR_VER is the version of the winampGeneralPurposePlugin (GPP) structure
winampGeneralPurposePlugin plugin = { GPPHDR_VER, const_cast<char *>(PLUGIN_NAME), init, config, quit, 0, 0 };

// Config settings
int bShowSlider = 0;

LPCSTR ini_file = 0;
void config_read()
{
	// this will only correctly work with Winamp 2.9+/5.x+
	// see IPC_GETINIFILE for a way to query the location of Winamp.ini correctly
	// whatever version of Winamp is being run on
	ini_file = (LPCSTR)SendMessage(plugin.hwndParent, WM_USER, 0, IPC_GETINIFILE);
	//if ((unsigned int)ini_file < 65536)
	//	ini_file = "winamp.ini";
	bShowSlider = GetPrivateProfileIntA(PLUGIN_NAME, "showSlider", bShowSlider, ini_file);
}

void config_write()
{
	char string[32] = { 0 };
	StringCchPrintfA(string, 32, "%d", bShowSlider);
	WritePrivateProfileStringA(PLUGIN_NAME, "showSlider", string, ini_file);
}

static void updateDiscordPresence()
{
	wchar_t wartist[512];
	wchar_t wtitle[512];

	extendedFileInfoStructW efs;
	efs.filename = (wchar_t *)SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_GET_PLAYING_FILENAME);
	efs.metadata = L"artist";
	efs.ret = wartist;
	efs.retlen = 512;
	SendMessage(plugin.hwndParent, WM_WA_IPC, (WPARAM)&efs, IPC_GET_EXTENDED_FILE_INFOW);
	efs.metadata = L"title";
	efs.ret = wtitle;
	SendMessage(plugin.hwndParent, WM_WA_IPC, (WPARAM)&efs, IPC_GET_EXTENDED_FILE_INFOW);

	char artist[512];
	char title[512];

	size_t len;
	wcstombs_s(&len, artist, sizeof artist, wartist, sizeof artist);
	wcstombs_s(&len, title, sizeof title, wtitle, sizeof title);
	

	int current = SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_GETOUTPUTTIME);
	if (current == -1) {
		current = 0;
	}

	int total = SendMessage(plugin.hwndParent, WM_WA_IPC, 2, IPC_GETOUTPUTTIME);
	int status = SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_ISPLAYING);

	char details[1027];
	sprintf_s(details, sizeof details, "%s - %s", artist, title);

	char state[1024];

#define TIME_BAR_LEN 17

	//char time_bar[TIME_BAR_LEN] = { '-', '-', '-', '-', '-', '-', '-', '-', '\0' };
	char time_bar[TIME_BAR_LEN] = { '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '\0' };

	if (current > 0) {
		current -= 1;
	}

	time_bar[int((TIME_BAR_LEN - 1) * current / total)] = '|';
	char slider[TIME_BAR_LEN + 3];
	sprintf_s(slider, sizeof slider, "[%s]", time_bar);

	char time_val[128];
	sprintf_s(time_val, sizeof time_val, "%d:%02d / %d:%02d", current / 1000 / 60, current / 1000 % 60, total / 1000 / 60, total / 1000 % 60);

	sprintf_s(state, sizeof state, "%s: %s",
		(status == 1 ? "Playing" : status == 3 ? "Paused" : status == 0 ? "Stopped" : "Error"),
		bShowSlider ? slider : time_val		
	);

	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.state = state;
	discordPresence.details = details;
	discordPresence.largeImageKey = "winamp";
	Discord_UpdatePresence(&discordPresence);
}

void update() {
	while(true) {
		updateDiscordPresence();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

extern "C" int init() {
	config_read();

	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);

	std::thread(update).detach();

	return 0;
}

BOOL CALLBACK ConfigProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		//MessageBox(plugin.hwndParent, L"InitDialog.", L"", MB_OK);
		CheckDlgButton(hwndDlg, IDC_CHECK1, bShowSlider);
		break;
	case WM_COMMAND:
		//MessageBox(plugin.hwndParent, L"Command.", L"", MB_OK);
		switch (LOWORD(wParam))
		{
		case IDOK:
			bShowSlider = IsDlgButtonChecked(hwndDlg, IDC_CHECK1);
			config_write();
			DestroyWindow(hwndDlg);
			return TRUE;
		case IDCANCEL:
			DestroyWindow(hwndDlg);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

extern "C" void config() {
	//MessageBox(plugin.hwndParent, L"Discord Rich Presence Plugin\nDeveloped by James Smith, 2018.", L"", MB_OK);
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOGCONFIG), 0, ConfigProc);
}

extern "C" void quit() {
	config_write();
	Discord_Shutdown();
}


// This is an export function called by winamp which returns this plugin info.
// We wrap the code in 'extern "C"' to ensure the export isn't mangled if used in a CPP file.
extern "C" __declspec(dllexport) winampGeneralPurposePlugin * winampGetGeneralPurposePlugin() {
	return &plugin;
}
