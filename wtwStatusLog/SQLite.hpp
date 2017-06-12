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

#ifndef SQLITE_HPP
#define SQLITE_HPP

#include <sqlite3.h>
#include <string>

class SQLException {
public:
	SQLException(const std::wstring& m) : msg(m) { 
		debug(msg.c_str());
	}

	SQLException(sqlite3* db) { 
		if(db != 0) {
			msg = (const wchar_t*)sqlite3_errmsg16(db);
		} else {
			msg = L"Database not connected";
		}
		debug(msg.c_str());
	}

	const wchar_t* message() const { 
		return msg.c_str(); 
	}

private:
	static void debug(const wchar_t* m) {
		__LOG_F(wtw, WTW_LOG_LEVEL_DEBUG, L"STLG", L"SQL Exception: %s", m);
		::OutputDebugStringW(L"SQLException: ");
		::OutputDebugStringW(m);
		::OutputDebugStringW(L"\n");
	}

	std::wstring msg;
};

class SQLDatabase {
public:
	SQLDatabase() : db(0) { }
	~SQLDatabase() { close(); }

	void open(const std::wstring& path) throw(...);
	void close() throw();
	int changes() throw(...);

	sqlite3* database() { return db; }

	sqlite3* operator() () {
		return db;
	}

private:
	sqlite3* db;
};

class SQLStatement {
public:
	SQLStatement() : st(0), db(0) { }
	~SQLStatement() { destroy(); }

	void create(sqlite3* db, const wchar_t* query) throw(...);
	void destroy() throw();
	int step() throw(...);
	
	void bind(int pos) throw(...);
	void bind(int pos, int v) throw(...);
	void bind(int pos, __int64 v) throw(...);
	void bind(int pos, const wchar_t* v) throw(...);

	const wchar_t* getString(int column) throw(...);
	int getInt(int column) throw(...);
	__int64 getInt64(int column) throw(...);

	int getColumnCount() throw(...);
	const wchar_t* getColumnName(int n) throw(...);
	int getColumnType(int n) throw(...);
private:
	sqlite3_stmt* st;
	sqlite3* db;
};

#endif
