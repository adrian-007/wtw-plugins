/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "..\wtwPlugin.h"
#include "..\wtwFile.h"
#include "Core.h"
#include "BaseObject.h"

namespace wtw 
{
	class LIBWTW_API CFile : public CBaseObject
	{
	public:
		CFile();
		virtual ~CFile();

	protected:
		void *m_pFile;

	public:
		bool open(const wchar_t *filePath, int bRead, int bWrite, int bCreate, int bTruncate, HRESULT *pExtInfo = NULL, int bNoEnc = 0);
		bool close();
		bool read(void *pData, DWORD nData, DWORD &bytesProcessed);
		bool write(void *pData, DWORD nData, DWORD &bytesProcessed);
		QWORD seek(long long offset, int from = WFS_CURRENT);
		QWORD getLength();
		QWORD seekToEnd();
		QWORD seekToBegin();
		bool encrypt();
		bool decrypt();
		DWORD getSectorSize();
		bool setSectorSize(DWORD size);

	public:
		static QWORD getLength(const wchar_t *path);
		static DWORD getAttributes(const wchar_t *path);
		static bool fileExists(const wchar_t *path);
		static bool fileDelete(const wchar_t *path);

		enum
		{
			WTW_INVALID_FILE_LENGTH = -1LL,
			WTW_INVALID_ATTRIBUTES = -1L,
		};

		enum
		{
			WTW_FILE_ATTRIBUTE_READONLY            = 0x00000001 , 
			WTW_FILE_ATTRIBUTE_HIDDEN              = 0x00000002 ,
			WTW_FILE_ATTRIBUTE_SYSTEM              = 0x00000004 ,
			WTW_FILE_ATTRIBUTE_DIRECTORY           = 0x00000010 , 
			WTW_FILE_ATTRIBUTE_ARCHIVE             = 0x00000020 , 
			WTW_FILE_ATTRIBUTE_DEVICE              = 0x00000040 , 
			WTW_FILE_ATTRIBUTE_NORMAL              = 0x00000080 , 
			WTW_FILE_ATTRIBUTE_TEMPORARY           = 0x00000100 , 
			WTW_FILE_ATTRIBUTE_SPARSE_FILE         = 0x00000200 , 
			WTW_FILE_ATTRIBUTE_REPARSE_POINT       = 0x00000400 , 
			WTW_FILE_ATTRIBUTE_COMPRESSED          = 0x00000800 , 
			WTW_FILE_ATTRIBUTE_OFFLINE             = 0x00001000 , 
			WTW_FILE_ATTRIBUTE_NOT_CONTENT_INDEXED = 0x00002000 , 
			WTW_FILE_ATTRIBUTE_ENCRYPTED           = 0x00004000 , 
			WTW_FILE_ATTRIBUTE_VIRTUAL             = 0x00010000 , 
		} attributes;
	};
};