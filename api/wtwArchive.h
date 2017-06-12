/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "wtwProtocol.h"

struct wtwArchiveEntrySms 
{
	__wtwStdStructDefs(wtwArchiveEntrySms);

	int				structSize;

	const wchar_t * body;
	const wchar_t * extra;
	const wchar_t * phone;

	DWORD			flags;
	QWORD			time;

	DWORD			writeFlags;
};

#define WTW_ARCH_WRITE_SMS			L"Ar/WriteSMS"
#define WTW_ARCH_WRITE_MESSAGE_EX	L"Ar/WriteMessageEx"

struct wtwArchiveEntryMessageEx 
{
	__wtwStdStructDefs(wtwArchiveEntryMessageEx);

	int				structSize;
	__int64			chatId;			// to jest parametr in i prawie out. Jesli potrzebujemy nowego ID
									// to ustawiamy na -1, wraz z pierwsza wypowiedzia w rozmowie;
									// program wygeneruje nowy ID i zwroci go nam przez callback w lp (wp==2);
									// i tego ID uzywamy w pozniejszych wywolaniach funkcji, poki
									// nie rozpoczniemy nowej rozmowy; Takze, przy nowych -1;
	__int64			tempId;			// jesli chatId == -1, to ten parametr jest uzywany do identyfikacji 
									// w¹tku, jesli w kolejce zapisu jest kilka wiadomosci
									// z cid == -1 i zostanie wygenerowany nocy cid (i zwrocny callbackiem)
									// to wszystkie wiadomosci z tempId takim samym jak piewsza wiadomosc
									// automatycznie otrzymaja nowy cid

	DWORD			flags;

	wtwMessageDef	message;

	WTWFUNCTION		cb;				// ten callback MUSI byc thread safe
	void *			cbData;

	void * reserved;
};

#define WTW_ARCH_TRANSACTION			L"Arch/Transaction" // wP - WATR_*

#define WATR_BEGIN		0x01
#define WATR_COMMIT		0x02
#define WATR_ROLLBACK	0x03

#define WTW_ARCH_PAGE_ACTION_SHOW           1	// Pokazac okienko (lP - wtwContactDef* or NULL)
#define WTW_ARCH_PAGE_ACTION_HIDE           2	// Schowac okienko

#define WTW_ARCH_PAGE_ACTION_DESTROY        5	// zniszczyc nasze okno arch, bo
												// okno archiwum jest zamykane
#define WTW_ARCH_PAGE_ACTION_MOVE           6	// Przesun¹æ okno

#define WTW_ARCH_PAGE_ACTION_FILTER         8	// lparam - wtwContactDef*, jesli trzeba 
												// odfiltrowac dane dlakonretnego kontaktu
												// lub NULL, ktory oznacza "pokaz wszystko"
												// event wysylany tylko jesli archiwum jest juz otwarte, dana
												// strona wyswietlona, i zachodzi potrzeba zmiany filtra
												// tj kliknieto "archiwum" na innym kontakcie;

/////////////////////////////////////////////////////////////////////////////////
//
// WTW_ADD_ARCH_PAGE
// 
// wParam - hInstance
// lParam - wtwOptionpadeDef*
// 

#define WTW_ADD_ARCH_PAGE                   L"WTW/AddArchPage"

/////////////////////////////////////////////////////////////////////////////////
//
// WTW_REMOVE_ARCH_PAGES
// 
// wParam - hInstance
// lParam - NULL
// 

#define WTW_REMOVE_ARCH_PAGES           L"WTW/RemoveArchPages"

#define WTW_ARCH_PAGE_ID_CHATS			L"WTW/ArchPage/Chats"
#define WTW_ARCH_PAGE_ID_MESSAGES		L"WTW/ArchPage/Msgs"
#define WTW_ARCH_PAGE_ID_CONFERENCE		L"WTW/ArchPage/Conf"
#define WTW_ARCH_PAGE_ID_SMS			L"WTW/ArchPage/SMS"
#define WTW_ARCH_PAGE_ID_MAINTENANCE	L"WTW/ArchPage/Maint"
#define WTW_ARCH_PAGE_ID_STATS			L"WTW/ArchPage/Stats"


struct wtwChatExport
{
	__wtwStdStructDefs(wtwChatExport);

	int				structSize;

	wtwMessageDef *	pMessages;
	int				iMessages;

	void *			pBuffer;	// wtw::CBuffer*

	const wchar_t * profileName;

	DWORD			flags;
};

#define WTW_CHAT_EXPORT L"WTW/ChatExport" // wp* - wtwChatExport*

struct wtwArchiveSyncEx
{
	__wtwStdStructDefs(wtwArchiveSyncEx);

	int				structSize;

	wtwMessageDef *	pMessages; // messages MUST (!) have UMID and UCID set
	int				iMessages;

	DWORD			flags;

	void *			reserved1;
	void *			reserved2;

	WTWFUNCTION		cb;
	void *			cbData;
};

#define WTW_ARCHIVE_SYNC L"WTW/ArchiveSync" // wp* - wtwArchiveSync*, executed in caller's thread ASYNCHRONOUSLY!