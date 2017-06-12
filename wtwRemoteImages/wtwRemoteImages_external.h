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

#ifndef WTW_REMOTE_IMAGES_EXTERNAL_H
#define WTW_REMOTE_IMAGES_EXTERNAL_H

#ifdef _WIN32

typedef struct
{
	const wchar_t*	filePath;		// sciezka do pliku ktory chcemy wyslac

	const wchar_t*	contactId;		// id kontaktu do ktorego chcemy wyslac link do danego pliku
	const wchar_t*	netClass;		// siec
	int				netId;			// identyfikator sieci
}wtwRemoteImagesFileV1;

typedef wtwRemoteImagesFileV1 wtwRemoteImagesFile;

#define WTW_REMOTE_IMAGES_SEND_IMAGE L"wtwRemoteImages/sendImage"

typedef struct
{
	int				structSize;

	wchar_t*		serviceName;	// nazwa bierzacego dostawcy uslugi przechowujacej obazki
	int				serviceNameLen;

	wchar_t*		icon16_id;
	int				icon16_len;

	wchar_t*		icon24_id;
	int				icon24_len;

	wchar_t*		icon32_id;
	int				icon32_len;

	wchar_t*		icon48_id;
	int				icon48_len;
} wtwRemoteImagesInfo;

#define WTW_REMOTE_IMAGES_QUERY_INFO L"wtwRemoteImages/queryInfo"

#else

#error Unsupported platform/missing WindowsSDK

#endif // _WIN32

#endif // WTW_REMOTE_IMAGES_EXTERNAL_H
