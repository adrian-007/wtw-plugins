/*
 * Copyright (C) 2009-2017 adrian_007, adrian-007 on o2 point pl
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "stdinc.h"
#include <ctime>
#include "zip.h"
#include "resource.h"
#include "../pluginInfo.h"

WTWFUNCTIONS* wtw = 0;
HINSTANCE ghInstance = 0;
void* config = 0;

WTWPLUGINFO plugInfo =
{
	sizeof(WTWPLUGINFO),											// rozmiar struktury
	L"wtwConfigBackup",											// nazwa wtyczki
	L"Automatyczne tworzenie kopii zapasowych plików profilu",	// opis wtyczki
	__COPYRIGHTS(2009),											// prawa autorskie
	__AUTHOR_NAME,												// autor
	__AUTHOR_CONTACT,											// dane do kontaktu z autorem
	__AUTHOR_WEBSITE,											// strona www autora
	__AUTOUPDATE_URL(L"wtwConfigBackup.xml"),					// url do pliku xml z danymi do autoupdate
	PLUGIN_API_VERSION,											// wersja api z ktora zostal skompilowany plug
	MAKE_QWORD(0, 1, 0, 3),										// wersja plug'a
	WTW_CLASS_UTILITY,											// klasa plug'a
	0,															// fcja ktora wtw wywola jezeli user bedzie chcial zobaczyc about plug'a
	L"{575CED39-D2A1-4382-AEA5-30349A3CB6D8}",					// GUID, wymagane...
	0,															// zaleznosci (tablica GUID'ow, ostatnia pozycja MA byc 0
	0, 0, 0, 0													// zarezerwowane
};

bool __stdcall DllMain(HANDLE hInst, DWORD reason, LPVOID /*reserved*/)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
	{
							   ghInstance = (HINSTANCE)hInst;
							   break;
	}
	case DLL_PROCESS_DETACH:
	{
							   ghInstance = 0;
							   break;
	}
	default: break;
	}
	return true;
}

std::wstring getRootDir()
{
	std::wstring rootDir;

	wtwDirectoryInfo di;

	di.dirType = WTW_DIRECTORY_PROFILE;
	di.flags = WTW_DIRECTORY_FLAG_FULLPATH;
	di.bi.bufferSize = -1;

	if (SUCCEEDED(wtw->fnCall(WTW_GET_DIRECTORY_LOCATION, di, 0)) && di.bi.bufferSize > 0)
	{
		di.bi.pBuffer = new wchar_t[di.bi.bufferSize];
		if (SUCCEEDED(wtw->fnCall(WTW_GET_DIRECTORY_LOCATION, di, 0)))
		{
			rootDir = di.bi.pBuffer;
		}
		delete di.bi.pBuffer;
	}

	return rootDir;
}

std::wstring getArchiveName()
{
	wchar_t buffer[256] = { 0 };
	time_t rawtime;
	tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	wcsftime(buffer, 256, L"config_%Y-%m-%d_%H.%M.%S.zip", timeinfo);
	return buffer;
}

void addToZip(HZIP& zip, const std::wstring& rootDir, std::wstring filter)
{
	WIN32_FIND_DATA data;
	HANDLE hFind;

	filter = rootDir + filter;

	hFind = FindFirstFile(filter.c_str(), &data);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			ZipAdd(zip, data.cFileName, std::wstring(rootDir + data.cFileName).c_str());
		} while (FindNextFile(hFind, &data));

		FindClose(hFind);
	}
}

std::wstring createBackup()
{
	std::wstring rootPath = getRootDir();
	std::wstring rootDir = rootPath + L"ConfigBackups\\";
	std::wstring zipName = getArchiveName();
	std::wstring zipPath = rootDir + zipName;

	::CreateDirectoryW(rootDir.c_str(), 0);

	HZIP zip = CreateZip(zipPath.c_str(), 0);

	addToZip(zip, rootPath, L"*.config");
	addToZip(zip, rootPath, L"*.roster");
	addToZip(zip, rootPath, L"*.xml");
	addToZip(zip, rootPath, L"*.bin");
	addToZip(zip, rootPath, L"*.db");

	CloseZip(zip);

	__LOG_F(wtw, WTW_LOG_LEVEL_INFO, L"WCBP", L"Config backup created (\\ConfigBackups\\%s)", zipName.c_str());
	cfgSetInt64(L"lastBackup", time(0));

	return zipName;
}

#define MENU_ROOT L"wtwConfigBackup/root"
#define MENU_CREATE_BACKUP L"wtwConfigBackup/create"
#define MENU_DELETE_OLDER_THAN_7DAYS L"wtwConfigBackup/del7day"
#define PLUGIN_ICON L"wtwConfigBackup/Icon"

unsigned __int64 getTimeDiffSeconds(FILETIME& ft) {
	FILETIME ftNow = { 0 };
	SYSTEMTIME st = { 0 };
	::GetSystemTime(&st);
	::SystemTimeToFileTime(&st, &ftNow);

	unsigned __int64 now = ((static_cast<unsigned __int64>(ftNow.dwHighDateTime)) << 32) + ftNow.dwLowDateTime;
	unsigned __int64 fts = ((static_cast<unsigned __int64>(ft.dwHighDateTime)) << 32) + ft.dwLowDateTime;

	return (now - fts) / static_cast<__int64>(10000000);
}

WTW_PTR MenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void*)
{
	wtwMenuPopupInfo* nfo = (wtwMenuPopupInfo*)lParam;
	wtwMenuItemDef* menu = (wtwMenuItemDef*)wParam;

	switch (reinterpret_cast<int>(menu->ownerData))
	{
	case 1:
	{
			  std::wstring msg = createBackup();

			  msg = L"Utworzono kopię zapasową ustawień profilu - " + msg;

			  {
				  wtwTrayNotifyDef nt;
				  initStruct(nt);
				  nt.textMain = L"wtwConfigBackup";
				  nt.textLower = msg.c_str();
				  nt.iconId = PLUGIN_ICON;
				  nt.graphType = WTW_TN_GRAPH_TYPE_SKINID;
				  nt.flags = WTW_TN_FLAG_TXT_MULTILINE | WTW_TN_FLAG_HDR_MULTILINE;

				  wtw->fnCall(WTW_SHOW_STANDARD_NOTIFY, reinterpret_cast<WTW_PARAM>(&nt), NULL);
			  }
			  break;
	}
	case 2:
	{
			  std::wstring rootDir = getRootDir();
			  rootDir += L"ConfigBackups\\";
			  std::wstring filter = rootDir + L"*.zip";

			  WIN32_FIND_DATA data;
			  HANDLE hFind;

			  hFind = FindFirstFile(filter.c_str(), &data);
			  if (hFind != INVALID_HANDLE_VALUE)
			  {
				  do
				  {
					  unsigned __int64 diff = getTimeDiffSeconds(data.ftCreationTime);
					  diff /= (3600 * 24);

					  if (diff > 7) {
						  std::wstring path = rootDir;
						  path += data.cFileName;

						  if (::DeleteFile(path.c_str())) {
							  __LOG_F(wtw, WTW_LOG_LEVEL_INFO, L"WCBP", L"File '%s' created %d days ago, removing...", data.cFileName, diff);
						  }
					  }
				  } while (FindNextFile(hFind, &data));

				  FindClose(hFind);
			  }
			  break;
	}
	default:
	{
	}
	}
	return 0;
}

extern "C"
{

	WTWPLUGINFO* __stdcall queryPlugInfo(DWORD /*apiVersion*/, DWORD /*masterVersion*/)
	{
		return &plugInfo;
	}

	int __stdcall pluginLoad(DWORD /*callReason*/, WTWFUNCTIONS* f)
	{
		wtw = f;
		{
			wtwMyConfigFile pBuff;
			initStruct(pBuff);

			pBuff.bufferSize = MAX_PATH + 1;
			pBuff.pBuffer = new wchar_t[MAX_PATH + 1];

			wtw->fnCall(WTW_SETTINGS_GET_MY_CONFIG_FILE, reinterpret_cast<WTW_PARAM>(&pBuff), reinterpret_cast<WTW_PARAM>(ghInstance));

			if (wtw->fnCall(WTW_SETTINGS_INIT_EX, reinterpret_cast<WTW_PARAM>(pBuff.pBuffer), reinterpret_cast<WTW_PARAM>(&config)) != S_OK)
			{
				config = 0;
			}
			else
			{
				wtw->fnCall(WTW_SETTINGS_READ, (WTW_PARAM)config, NULL);
			}

			delete[] pBuff.pBuffer;
		}

		{
			wtwGraphics icon;
			initStruct(icon);
			icon.graphId = PLUGIN_ICON;
			icon.resourceId = MAKEINTRESOURCE(IDB_PNG1);
			icon.imageType = 0;
			icon.hInst = ghInstance;
			wtw->fnCall(WTW_GRAPH_LOAD, (WTW_PARAM)&icon, 0);
		}

		{
			wtwMenuItemDef mi;
			initStruct(mi);
			mi.menuID = WTW_MENU_ID_MAIN_OPT;
			//mi.itemId		= MENU_ROOT;
			mi.hModule = ghInstance;
			//mi.menuCaption  = L"Kopia zapasowa";
			//mi.flags		= WTW_MENU_ITEM_FLAG_POPUP;
			mi.callback = MenuCallback;
			mi.iconId = PLUGIN_ICON;

			//wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

			//mi.flags		= 0;
			//mi.menuID		= MENU_ROOT;

			mi.itemId = MENU_CREATE_BACKUP;
			mi.menuCaption = L"Utwórz kopię plików konf.";
			mi.ownerData = reinterpret_cast<void*>(1);
			wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);

			mi.itemId = MENU_DELETE_OLDER_THAN_7DAYS;
			mi.menuCaption = L"Usuń kopie starsze niż 7 dni";
			mi.ownerData = reinterpret_cast<void*>(2);
			wtw->fnCall(WTW_MENU_ITEM_ADD, reinterpret_cast<WTW_PARAM>(&mi), 0);
		}

		time_t now = time(0);
		if ((now - cfgGetInt64(L"lastBackup", 0)) > (3600 * 12))
		{
			createBackup();
		}

		return 0;
	}

	int __stdcall pluginUnload(DWORD /*callReason*/)
	{
		wtw->fnCall(WTW_MENU_ITEM_CLEAR, reinterpret_cast<WTW_PARAM>(ghInstance), 0);

		wtw->fnCall(WTW_SETTINGS_WRITE, reinterpret_cast<WTW_PARAM>(config), 0);
		wtw->fnCall(WTW_SETTINGS_DESTROY, reinterpret_cast<WTW_PARAM>(config), reinterpret_cast<WTW_PARAM>(ghInstance));
		config = 0;

		wtw = 0;
		return 0;
	}

} // extern "C"
