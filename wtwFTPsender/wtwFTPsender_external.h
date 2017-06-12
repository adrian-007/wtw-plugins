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

#ifndef WTW_FTP_SENDER_EXTERNAL_H
#define WTW_FTP_SENDER_EXTERNAL_H
#ifdef _WIN32

struct wtwFtpFile {
	const wchar_t*	filePath;		// sciezka do pliku ktory chcemy wyslac

	const wchar_t*	contactId;		// id kontaktu do ktorego chcemy wyslac link do danego pliku
	const wchar_t*	netClass;		// siec
	int				netId;			// identyfikator sieci
};

#define WTW_FTP_SENDER_SEND_FILE L"wtwFTPsender/sendFile"

#endif // _WIN32
#endif // WTW_FTP_SENDER_EXTERNAL_H
