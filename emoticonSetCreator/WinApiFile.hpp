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

#ifndef WINAPI_FILE_HPP
#define WINAPI_FILE_HPP

#include <windows.h>
#include <string>

class WinApiFile {
public:
	enum {
		READ_MODE = GENERIC_READ,
		WRITE_MODE = GENERIC_WRITE,
		ALWAYS_OPEN = OPEN_ALWAYS,
		OPEN_IF_EXIST = OPEN_EXISTING,
		ALWAYS_CREATE = CREATE_ALWAYS
	};

	WinApiFile() {
		init();
	}
	WinApiFile(const std::wstring& path, DWORD mode = READ_MODE | WRITE_MODE, DWORD openRule = ALWAYS_OPEN) {
		init();
		open(path, mode, openRule);
	}
	~WinApiFile() {
		close();
	}

	bool valid() const { 
		return hFile != INVALID_HANDLE_VALUE; 
	}
	bool good() const {
		DWORD pos = SetFilePointer(hFile, 0, 0, FILE_CURRENT);
		if(pos != INVALID_SET_FILE_POINTER) {
			return pos < getFileSize();
		}
		return false; 
	}
	DWORD getFileSize() const { 
		DWORD size = GetFileSize(hFile, 0);
		if(size == 0xFFFFFFFF)
			size = -1;
		return size;
	}

	bool open(const std::wstring& path, DWORD mode = READ_MODE | WRITE_MODE, DWORD openRule = ALWAYS_OPEN) {
		if(valid())
			return false;
		hFile = CreateFile(path.c_str(), mode, 0, 0, openRule, FILE_ATTRIBUTE_NORMAL, 0);
		return hFile != INVALID_HANDLE_VALUE;
	}
	void close() {
		if(hFile != INVALID_HANDLE_VALUE) {
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
		}
	}

	//general purpose read/write
	DWORD write(const void* data, DWORD size) {
		DWORD tmp = 0;
		if(size > 0)
			WriteFile(hFile, data, size, &tmp, 0);
		return tmp;
	}
	DWORD read(void* buffer, DWORD size) {
		DWORD tmp = 0;
		if(size > 0)
			ReadFile(hFile, buffer, size, &tmp, 0);
		return tmp;
	}

	// operators
	// 1 byte
	WinApiFile& operator << (char value)					{ return generic_write(&value, sizeof(value)); }
	WinApiFile& operator << (unsigned char value)			{ return generic_write(&value, sizeof(value)); }
	WinApiFile& operator >> (char& value)					{ return generic_read(&value, sizeof(value)); }
	WinApiFile& operator >> (unsigned char& value)			{ return generic_read(&value, sizeof(value)); }
	
	WinApiFile& operator << (bool value)					{ return generic_write(&value, sizeof(value)); }
	WinApiFile& operator >> (bool& value)					{ return generic_read(&value, sizeof(value)); }

	// 2 byte
	WinApiFile& operator << (short value)					{ return generic_write(&value, sizeof(value)); }
	WinApiFile& operator << (unsigned short value)			{ return generic_write(&value, sizeof(value)); }
	WinApiFile& operator >> (short& value)					{ return generic_read(&value, sizeof(value)); }
	WinApiFile& operator >> (unsigned short& value)			{ return generic_read(&value, sizeof(value)); }
	
	WinApiFile& operator << (wchar_t value)					{ return generic_write(&value, sizeof(value)); }
	WinApiFile& operator >> (wchar_t& value)				{ return generic_read(&value, sizeof(value)); }

	//4 byte
	WinApiFile& operator << (int value)						{ return generic_write(&value, sizeof(value)); }
	WinApiFile& operator << (unsigned int value)			{ return generic_write(&value, sizeof(value)); }
	WinApiFile& operator >> (int& value)					{ return generic_read(&value, sizeof(value)); }
	WinApiFile& operator >> (unsigned int& value)			{ return generic_read(&value, sizeof(value)); }

	//8 byte
	WinApiFile& operator << (long value)					{ return generic_write(&value, sizeof(value)); }
	WinApiFile& operator << (unsigned long value)			{ return generic_write(&value, sizeof(value)); }
	WinApiFile& operator >> (long& value)					{ return generic_read(&value, sizeof(value)); }
	WinApiFile& operator >> (unsigned long& value)			{ return generic_read(&value, sizeof(value)); }

	//16 byte
	WinApiFile& operator << (long long value)				{ return generic_write(&value, sizeof(value)); }
	WinApiFile& operator << (unsigned long long value)		{ return generic_write(&value, sizeof(value)); }
	WinApiFile& operator >> (long long& value)				{ return generic_read(&value, sizeof(value)); }
	WinApiFile& operator >> (unsigned long long& value)		{ return generic_read(&value, sizeof(value)); }

	//strings - resize with size you want to read/write
	WinApiFile& operator << (const std::string& value)		{ return generic_write(&value[0], value.size()); }
	WinApiFile& operator << (const std::wstring& value)		{ return generic_write(&value[0], value.size() * sizeof(wchar_t)); }
	WinApiFile& operator >> (std::string& value)			{ return generic_read(&value[0], value.size()); }
	WinApiFile& operator >> (std::wstring& value)			{ return generic_read(&value[0], value.size() * sizeof(wchar_t)); }

	//utils
	static bool isUnicodeFile(const std::wstring& path) {
		WinApiFile file;
		file.open(path, WinApiFile::READ_MODE, WinApiFile::OPEN_IF_EXIST);
		wchar_t tag;
		file >> tag;
		if(tag == 65279 || tag == 65534)
			return true;
		return false;
	}
private:
	template<typename T>
	inline WinApiFile& generic_write(const T* value, DWORD size) {
		write(value, size);
		return *this;
	}
	template<typename T>
	inline WinApiFile& generic_read(T* buf, DWORD size) {
		read(buf, size);
		return *this;
	}

	inline void init() { 
		hFile = INVALID_HANDLE_VALUE; 
	}

	HANDLE hFile;
};

#endif
