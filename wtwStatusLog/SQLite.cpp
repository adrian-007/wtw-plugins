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
#include "SQLite.hpp"

void SQLDatabase::open(const std::wstring& path) throw(...) {
	close();

	int ret = sqlite3_open16((void*)path.c_str(), &db);

	if(ret != SQLITE_OK) {
		SQLException ex(db);
		db = 0;
		throw ex;
	}
}

void SQLDatabase::close() throw() {
	if(db != 0) {
		sqlite3_close(db);
	}
	db = 0;
}

int SQLDatabase::changes() throw(...) {
	if(db == 0) {
		throw SQLException(L"Database not connected");
	}

	int ret = sqlite3_changes(db);
	return ret;
}

void SQLStatement::create(sqlite3* db, const wchar_t* query) throw(...) {
	destroy();
	this->db = db;
	if(this->db == 0) {
		throw SQLException(L"Database not connected");
	}

	if(sqlite3_prepare16(db, (void*)query, -1, &st, 0) != SQLITE_OK) {
		st = 0;
		throw SQLException(db);
	}
	if(st == 0) {
		throw SQLException(L"Unknown exception while creating statement");
	}
}

void SQLStatement::destroy() throw() {
	if(st != 0) {
		sqlite3_finalize(st);
		st = 0;
	}
	db = 0;
}

int SQLStatement::step() throw(...) {
	if(st == 0) {
		throw SQLException(L"Can't step statement that hasn't been created");
	}

	int ret = sqlite3_step(st);
	if(ret == SQLITE_ERROR) {
		throw SQLException(db);
	}

	return ret;
}

void SQLStatement::bind(int pos) throw(...) {
	if(sqlite3_bind_null(st, pos) != SQLITE_OK) {
		throw SQLException(db);
	}
}

void SQLStatement::bind(int pos, int v) throw(...) {
	if(sqlite3_bind_int(st, pos, v) != SQLITE_OK) {
		throw SQLException(db);
	}
}

void SQLStatement::bind(int pos, __int64 v) throw(...) {
	if(sqlite3_bind_int64(st, pos, v) != SQLITE_OK) {
		throw SQLException(db);
	}
}

void SQLStatement::bind(int pos, const wchar_t* v) throw(...) {
	if(st == 0) {
		throw SQLException(L"Can't get column value from statement that hasn't been created");
	}

	if(v == 0) {
		bind(pos);
	} else {
		if(sqlite3_bind_text16(st, pos, (const void*)v, wcslen(v) * sizeof(wchar_t), SQLITE_STATIC) != SQLITE_OK) {
			throw SQLException(db);
		}
	}
}

const wchar_t* SQLStatement::getString(int column) throw(...) {
	if(st == 0) {
		throw SQLException(L"Can't get column value from statement that hasn't been created");
	}
	return (const wchar_t*)sqlite3_column_text16(st, column);
}

int SQLStatement::getInt(int column) throw(...) {
	if(st == 0) {
		throw SQLException(L"Can't get column value from statement that hasn't been created");
	}
	return sqlite3_column_int(st, column);
}

__int64 SQLStatement::getInt64(int column) throw(...) {
	if(st == 0) {
		throw SQLException(L"Can't get column value from statement that hasn't been created");
	}
	return sqlite3_column_int64(st, column);
}

int SQLStatement::getColumnCount() throw(...) {
	if(st == 0) {
		throw SQLException(L"Can't get column count from statement that hasn't been created");
	}
	return sqlite3_column_count(st);
}

const wchar_t* SQLStatement::getColumnName(int n) throw(...) {
	if(st == 0) {
		throw SQLException(L"Can't get column name from statement that hasn't been created");
	}

	const wchar_t* ret = (const wchar_t*)sqlite3_column_name16(st, n);
	if(ret == 0) {
		throw SQLException(L"Column doesn't exists");
	}
	return ret;
}

int SQLStatement::getColumnType(int n) throw(...) {
	if(st == 0) {
		throw SQLException(L"Can't get column type from statement that hasn't been created");
	}

	return sqlite3_column_type(st, n);
}
