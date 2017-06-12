/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "Globals.h"
#include "..\wtwPlugin.h"
#include "..\wtwCrypto.h"
#include "Core.h"
#include "File.h"
#include "String.h"
#include "BaseObject.h"

namespace wtw
{
	class LIBWTW_API CCryptoHash : public CBaseObject
	{
	public:
		CCryptoHash(int hashType);
		virtual ~CCryptoHash();

	public:
		WTW_PTR hash(const void * data, int dataLen, wtw::CString & hash);
		WTW_PTR hash(const void * data, int dataLen, char * hash, int hashLen);

		WTW_PTR init();
		WTW_PTR update(const void * data, int dataLen);
		WTW_PTR finish(char * hash, int hashLen);
		WTW_PTR finish(wtw::CString & hash);

	private:
		int m_hashType;
		void *m_ctx;
	};

	class LIBWTW_API CCryptoBlockAlgoEBC : public CBaseObject
	{
	protected:
		CCryptoBlockAlgoEBC();

	public:
		virtual ~CCryptoBlockAlgoEBC();

	public:

		virtual int getBlockLength() = 0;

		virtual HRESULT blockEncrypt(void * dataBlock, int dataBlockLen) = 0;
		virtual HRESULT blockDecrypt(void * dataBlock, int dataBlockLen) = 0;
	};

	class LIBWTW_API CCryptoBlockAlgoCBC : public CBaseObject
	{
	protected:
		CCryptoBlockAlgoCBC();

	public:
		virtual ~CCryptoBlockAlgoCBC();

	public:

		virtual int getBlockLength() = 0;

		virtual HRESULT blockEncrypt(void * dataBlock, int dataBlockLen, void *ivec) = 0;
		virtual HRESULT blockDecrypt(void * dataBlock, int dataBlockLen, void *ivec) = 0;
	};

	class LIBWTW_API CCryptoAES : public CCryptoBlockAlgoEBC, public CCryptoBlockAlgoCBC
	{
	protected:
		void *m_keyEnc;
		void *m_keyDec;

	public:
		CCryptoAES(const wchar_t *passPhrase, int b256Bit = 1);
		CCryptoAES(int keySizeInBits, void * pKey);
		virtual ~CCryptoAES();

	public:
		int getBlockLength();

		HRESULT blockEncrypt(void * dataBlock, int dataBlockLen);
		HRESULT blockDecrypt(void * dataBlock, int dataBlockLen);
		HRESULT blockEncrypt(void * dataBlock, int dataBlockLen, void *ivec);
		HRESULT blockDecrypt(void * dataBlock, int dataBlockLen, void *ivec);
	};
};