/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#define __UTILSAPI __cdecl

#include "Core.h"

namespace wtw
{

	class LIBWTW_API CXmlSax
	{
	protected:
		void * m_pParser;

	public:
		CXmlSax();
		virtual ~CXmlSax();

	public:
		HRESULT create(const wchar_t *pszEncoding = NULL, const wchar_t *pszSep = NULL);
		void destroy();

	public:
		HRESULT parse(const char *pszBuffer, int nLength = -1, bool bIsFinal = true);
		HRESULT parse(const wchar_t *pszBuffer, int nLength = -1, bool bIsFinal = true);
		HRESULT parseBuffer(int nLength, bool bIsFinal = true);
		void *getBuffer(int nLength);

	public:
		HRESULT enableStartElementHandler(bool bEnable = true);
		HRESULT enableEndElementHandler(bool bEnable = true);
		HRESULT enableElementHandler(bool bEnable = true);
		HRESULT enableCharacterDataHandler(bool bEnable = true);
		HRESULT enableProcessingInstructionHandler(bool bEnable = true);
		HRESULT enableCommentHandler(bool bEnable = true);
		HRESULT enableStartCdataSectionHandler(bool bEnable = true);
		HRESULT enableEndCdataSectionHandler(bool bEnable = true);
		HRESULT enableCdataSectionHandler(bool bEnable = true);
		HRESULT enableDefaultHandler(bool bEnable = true, bool bExpand = true);
		HRESULT enableExternalEntityRefHandler(bool bEnable = true);
		HRESULT enableUnknownEncodingHandler(bool bEnable = true);
		HRESULT enableStartNamespaceDeclHandler(bool bEnable = true);
		HRESULT enableEndNamespaceDeclHandler(bool bEnable = true);
		HRESULT enableNamespaceDeclHandler(bool bEnable = true);
		HRESULT enableXmlDeclHandler(bool bEnable = true);
		HRESULT enableStartDoctypeDeclHandler(bool bEnable = true);
		HRESULT enableEndDoctypeDeclHandler(bool bEnable = true);
		HRESULT enableDoctypeDeclHandler(bool bEnable = true);

	public:
		int getErrorCode();
		HRESULT getCurrentByteIndex(long &);
		HRESULT getCurrentLineNumber(int &);
		HRESULT getCurrentColumnNumber(int &);
		HRESULT getCurrentByteCount(int &);
		const wchar_t *getErrorString();

	public:
		static const wchar_t *getVersion();
		static void getVersion(int *pnMajor, int *pnMinor, int *pnMicro);
		static const wchar_t *getErrorString(int nError);

	protected:
		virtual void onStartElement(const wchar_t *pszName, const wchar_t **papszAttrs);
		virtual void onEndElement(const wchar_t *pszName);
		virtual void onCharacterData(const wchar_t *pszData, int nLength);
		virtual void onProcessingInstruction(const wchar_t *pszTarget, const wchar_t *pszData);
		virtual void onComment(const wchar_t *pszData);
		virtual void onStartCdataSection();
		virtual void onEndCdataSection();
		virtual void onDefault(const wchar_t *pszData, int nLength);
		virtual bool onExternalEntityRef(const wchar_t *pszContext, const wchar_t *pszBase, const wchar_t *pszSystemID, const wchar_t *pszPublicID);
		//virtual bool onUnknownEncoding(const wchar_t *pszName, XML_Encoding *pInfo);
		virtual void onStartNamespaceDecl(const wchar_t *pszPrefix, const wchar_t *pszURI);
		virtual void onEndNamespaceDecl(const wchar_t *pszPrefix);
		virtual void onXmlDecl(const wchar_t *pszVersion, const wchar_t *pszEncoding, bool bStandalone);
		virtual void onStartDoctypeDecl(const wchar_t *pszDoctypeName, const wchar_t *pszSysID, const wchar_t *pszPubID, bool bHasInternalSubset);
		virtual void onEndDoctypeDecl();

	protected:
		virtual void onPostCreate();

	private:
		static void __cdecl _startElementHandler(void *cbData, const wchar_t *pszName, const wchar_t **papszAttrs);
		static void __cdecl _endElementHandler(void *cbData,	const wchar_t *pszName);
		static void __cdecl _characterDataHandler(void *cbData, const wchar_t *pszData, int nLength);
		static void __cdecl _processingInstructionHandler(void *cbData, const wchar_t *pszTarget, const wchar_t *pszData);
		static void __cdecl _commentHandler(void *cbData,	const wchar_t *pszData);
		static void __cdecl _startCdataSectionHandler(void *cbData);
		static void __cdecl _endCdataSectionHandler(void *cbData);
		static void __cdecl _defaultHandler(void *cbData, const wchar_t *pszData, int nLength);
		static int __cdecl _externalEntityRefHandler(void *cbData, const wchar_t *pszContext, const wchar_t *pszBase, const wchar_t *pszSystemID, const wchar_t *pszPublicID);
		//static int __cdecl _unknownEncodingHandler(void *cbData, const wchar_t *pszName, XML_Encoding *pInfo);
		static void __cdecl _startNamespaceDeclHandler(void *cbData, const wchar_t *pszPrefix, const wchar_t *pszURI);
		static void __cdecl _endNamespaceDeclHandler(void *cbData, const wchar_t *pszPrefix);
		static void __cdecl _xmlDeclHandler(void *cbData, const wchar_t *pszVersion, const wchar_t *pszEncoding, int nStandalone);
		static void __cdecl _startDoctypeDeclHandler(void *cbData, const wchar_t *pszDoctypeName, const wchar_t *pszSysID, const wchar_t *pszPubID, int nHasInternalSubset);
		static void __cdecl _endDoctypeDeclHandler(void *cbData);
	};
}