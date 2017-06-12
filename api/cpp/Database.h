/*
** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "..\wtwPlugin.h"
#include "..\wtwOther.h"
#include "Core.h"
#include "File.h"
#include "String.h"
#include "BaseObject.h"

namespace wtw
{
	namespace db
	{
		class LIBWTW_API CConnection : public CBaseObject
		{
			friend class CCommand;

		protected:
			struct sqlite3 *m_db;
			int		m_bInTransaction;

		public:
			HRESULT getLastError(DWORD & code);
			HRESULT getLastError(const wchar_t **ret);

		public:
			HRESULT transactionBegin();
			HRESULT transactionCommit();
			HRESULT transactionRollback();
			HRESULT inTrasaction();

		public:
			CConnection();
			CConnection(const wchar_t * dbPath);
			virtual ~CConnection();
			HRESULT open(const wchar_t *dbPath);
			HRESULT close();
			HRESULT columnExists(const wchar_t *table, const wchar_t *column);
			HRESULT getLastInsertId(unsigned long long &id);
			HRESULT getLastInsertId(long long &id);
			HRESULT getLastInsertId(unsigned int &id);
			HRESULT getLastInsertId(int &id);
			bool	isOpened();
			HRESULT	releaseMemory();

		public:
			HRESULT executeNoRead(const wchar_t*sql);
			HRESULT executeAndRead(const wchar_t *sql, int & ret);
			HRESULT executeAndRead(const wchar_t *sql, long long & ret);
			HRESULT executeAndRead(const wchar_t *sql, double & ret);
			HRESULT executeAndRead(const wchar_t *sql, wtw::CString & ret);
			HRESULT executeAndRead(const wchar_t *sql, const wchar_t **);
		};
	
		class LIBWTW_API CConnection2 : public wtw::db::CConnection
		{
		public:
			CConnection2();
			CConnection2(const wchar_t * dbPath);
			virtual ~CConnection2();

		private:
			static int onBusyCallback(void *, int);
			int m_busyRetry;
			int m_bustDelay;

		public:
			HRESULT enableBusyHandler(int maxRetry, int delay);
		};

		class LIBWTW_API CCommand : public CBaseObject
		{
			friend class CReader;

		protected:
			CConnection &	m_con;
			struct sqlite3_stmt *m_statament;
			int				m_argc;
			int				m_refs;

		public:
			CCommand(CConnection & con, const wchar_t*sql);
			CCommand(CConnection & con);
			virtual ~CCommand();

			HRESULT prepare(const wchar_t *sql);
			HRESULT finalize();

			HRESULT executeNoRead();
			CReader executeReader();

			HRESULT bind(int index);	// 1 - based
			HRESULT bind(int index, const int & data);	// 1 - based
			HRESULT bind(int index, const long long & data);	// 1 - based
			HRESULT bind(int index, const double & data);	// 1 - based
			HRESULT bind(int index, const wchar_t *data, int dataLen = -1);	// 1 - based
			HRESULT bind(int index, const char *data, int dataLen = -1);	// 1 - based
			HRESULT bind(int index, const void *data, int dataLen);	// 1 - based

			HRESULT executeAndRead(int & ret);
			HRESULT executeAndRead(long long & ret);
			HRESULT executeAndRead(double & ret);
			HRESULT executeAndRead(wtw::CString & ret);
		};

		class LIBWTW_API CTransaction : public CBaseObject
		{
		private:
			CConnection &m_con;
			int m_inTrans;

		public:
			CTransaction(CConnection &con, int bStart = 1);
			virtual ~CTransaction();

			HRESULT begin();
			HRESULT commit();
			HRESULT rollback();
		};

		class LIBWTW_API CReader : public CBaseObject
		{
			friend class CCommand;

		protected:
			
			CCommand *m_cmd;
			CReader(CCommand *cmd);

		public:
			CReader();

			CReader(const CReader &copy);
			virtual ~CReader();

			int m_intptr;
			int m_rowCount;

			CReader& operator = (const CReader &copy);

			HRESULT read();
			HRESULT reset();
			HRESULT close();
			HRESULT count(int & count);
			HRESULT getRowNum(int & i);

			HRESULT hasNext();

			HRESULT getResult(int index, int &result, int defaultVal = 0);
			HRESULT getResult(int index, long long &result, long long defaultVal = 0LL);
			HRESULT getResult(int index, double &result, double defaultVal = 0.f);
			HRESULT getResult(int index, wtw::CString &result, const wtw::CString & defaultVal = L"");
			HRESULT getResult(int index, const wchar_t** result, const wchar_t *defaultVal = L"");
			HRESULT getResult(int index, const char** result, const char *defaultVal = "");
			HRESULT getResult(int index, void **data, int & dataLen);
			
			HRESULT getResult(const wchar_t *colName, void **data, int & dataLen);
			HRESULT getResult(const wchar_t *colName, int &result, int defaultVal = 0);
			HRESULT getResult(const wchar_t *colName, long long &result, long long defaultVal = 0LL);
			HRESULT getResult(const wchar_t *colName, double &result, double defaultVal = 0.f);
			HRESULT getResult(const wchar_t *colName, wtw::CString &result, const wtw::CString & defaultVal = L"");
			HRESULT getResult(const wchar_t *colName, const wchar_t** result, const wchar_t *defaultVal = L"");
			HRESULT getResult(const wchar_t *colName, const char** result, const char *defaultVal = "");
			
			HRESULT getColumnSize(int index, int & size);
			HRESULT getColumnCount(int &count);
			HRESULT getColumnName(int index, wtw::CString & name);
			HRESULT getColumnName(int index, const wchar_t **name);

		private:
			std::deque<void*> m_toFree;

			void releaseIntPointers();
		};
	};
};