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
#include "Logger.hpp"
#include <fstream>
#include <ctime>
#include <boost/format.hpp>
#include "SQLite.hpp"

#include "../utils/wtw.hpp"

#define WCHAR_BYTE_LEN(x) (wcslen(x) * sizeof(wchar_t))
Logger* Logger::inst = 0;

Logger::Logger() : hook(0) {
	wtw_t::paths::getPath(wtw, WTW_DIRECTORY_PROFILE, dbPath);

	dbPath += L".archive\\wtwStatusLog.sqlite";

	try {
		db.open(dbPath);
	} catch(const SQLException& /*ex*/) {
		return;
	}

	std::wstring query;

	SQLStatement stm;

	try {
		query =
			L"CREATE TABLE [contacts] (" \
			L"[id] INTEGER  NOT NULL DEFAULT 0," \
			L"[contactId] TEXT  NOT NULL," \
			L"[netClass] TEXT  NOT NULL," \
			L"[name] TEXT  NOT NULL," \
			L"PRIMARY KEY(contactId, netClass)" \
			L");";

		/*	L"create table wtw_status_log (" \
			L"id text," \
			L"netClass text," \
			L"name text," \
			L"status integer," \
			L"desc text," \
			L"time datetime);";*/
	
		stm.create(db.database(), query.c_str());
		stm.step();
		stm.create(db(), L"alter table contacts add column netId integer default 1;");
		stm.step();
	} catch(const SQLException& /*ex*/) {
		//__LOG_F(wtw, 1, L"STLG", L"Failed to create SQL table: %s", ex.message());
	}

	// fallback, alter tabeli
	try {
		stm.create(db(), L"alter table contacts add column netId integer default 1;");
		stm.step();
	} catch(const SQLException& /*ex*/) {
		//__LOG_F(wtw, 1, L"STLG", L"Failed to create SQL table: %s", ex.message());
	}

	try {
		query = 
			/*L"create trigger add_local_time after insert on wtw_status_log\n" \
			L"begin\n" \
			L" update wtw_status_log set time = datetime('now', 'localtime') where rowid = new.rowid;\n" \
			L"end;";*/
			
			L"create trigger setContactIndex after insert on contacts\n" \
			L"begin\n" \
 			L" update contacts set id = ((select max(id) from contacts) + 1) where rowid = new.rowid;\n" \
			L"end;";

		stm.create(db.database(), query.c_str());
		stm.step();
	} catch(const SQLException& /*ex*/) {
		//__LOG_F(wtw, 1, L"STLG", L"Failed to create trigger: %s", ex.message());
	}

	try {
		query =
			L"CREATE TABLE [logs] (" \
			L"[contactID] INTEGER  NOT NULL," \
			L"[status] INTEGER  NOT NULL," \
			L"[description] TEXT  NULL," \
			L"[time] DATETIME  NULL" \
			L");";
	
		stm.create(db.database(), query.c_str());
		stm.step();
	} catch(const SQLException& /*ex*/) {
		//__LOG_F(wtw, 1, L"STLG", L"Failed to create SQL table: %s", ex.message());
	}

	try {
		query =
			L"CREATE TABLE [tempLogs] (" \
			L"[id] TEXT  NOT NULL," \
			L"[netClass] TEXT NOT NULL," \
			L"[name] TEXT NOT NULL," \
			L"[status] INTEGER  NOT NULL," \
			L"[description] TEXT  NULL," \
			L"[time] DATETIME  NULL" \
			L")";
	
		stm.create(db.database(), query.c_str());
		stm.step();
	} catch(const SQLException& /*ex*/) {
		//__LOG_F(wtw, 1, L"STLG", L"Failed to create SQL table: %s", ex.message());
	}

	// fallback, alter tabeli
	try {
		stm.create(db(), L"alter table tempLogs add column netId integer default 1;");
		stm.step();
	} catch(const SQLException& /*ex*/) {
		//__LOG_F(wtw, 1, L"STLG", L"Failed to create SQL table: %s", ex.message());
	}
	/*
	// kod do przywracania poprzedniej wersji archiwum
	{
		SQLDatabase d;
		d.open(tmpPath);

		try {
			stm.create(db(), L"begin transaction;");
			stm.step();

			stm.create(d(), L"select id, netClass, name, status, desc, time from wtw_status_log;");

			while(true) {
				int rc = stm.step();
				if(rc == SQLITE_ROW) {
					try {
						SQLStatement us;
						us.create(db(), L"insert into tempLogs(id, netClass, name, status, description, time) values(?, ?, ?, ?, ?, ?);");
						us.bind(1, stm.getString(0));
						us.bind(2, stm.getString(1));
						us.bind(3, stm.getString(2));
						us.bind(4, stm.getString(3));
						us.bind(5, stm.getString(4));
						us.bind(6, stm.getString(5));
						us.step();
					} catch(const SQLException& ex) {
					//	__LOG_F(wtw, 1, L"STLG", L"Failed to select values from DB: %s", ex.message());
					}
					continue;
				} 

				break;
			}

			stm.create(db(), L"commit;");
			stm.step();
		} catch(const SQLException& ex) {
		//	__LOG_F(wtw, 1, L"STLG", L"Failed to select values from DB: %s", ex.message());
		}
		d.close();

		merge();
	}*/

	try {
		query =
			L"create trigger setLogTime after insert on tempLogs\n" \
			L"begin\n" \
			L" update tempLogs set time = datetime('now', 'localtime') where rowid = new.rowid;\n" \
			L"end;";
	
		stm.create(db.database(), query.c_str());
		stm.step();
	} catch(const SQLException& /*ex*/) {
		//__LOG_F(wtw, 1, L"STLG", L"Failed to create SQL trigger: %s", ex.message());
	}

	hook = wtw->evHook(WTW_ON_PROTOCOL_EVENT, Logger::dispatchProtocol, (void*)this);
}

Logger::~Logger() {
	merge();

	wtw->evUnhook(hook);
	hook = 0;

	db.close();
}

void Logger::merge() {
	try {
		SQLStatement stm;
		stm.create(db.database(), L"select id, netClass, name, netId from tempLogs group by id, netClass;");

		while(true) {
			int rc = stm.step();
			if(rc == SQLITE_ROW) {
				const wchar_t* id = stm.getString(0);
				const wchar_t* netClass = stm.getString(1);
				const wchar_t* name = stm.getString(2);
				int netId = stm.getInt(3);

				try {
					SQLStatement us;

					us.create(db(), L"update contacts set name = ?, netId = ? where contactId like ? and netClass like ?;");
					us.bind(1, name);
					us.bind(2, netId);
					us.bind(3, id);
					us.bind(4, netClass);
					us.step();

					if(db.changes() <= 0) {
						us.create(db(), L"insert into contacts(contactId, netClass, name, netId) values(?, ?, ?, ?);");
						us.bind(1, id);
						us.bind(2, netClass);
						us.bind(3, name);
						us.bind(4, netId);
						us.step();
					}

				} catch(const SQLException& /*ex*/) {
					//__LOG_F(wtw, 1, L"STLG", L"Failed to select values from DB: %s", ex.message());
				}
				continue;
			} 

			break;
		}
	} catch(const SQLException& /*ex*/) {
		//__LOG_F(wtw, 1, L"STLG", L"Failed to select values from DB: %s", ex.message());
	}

	try {
		SQLStatement stm;

		stm.create(db(), L"begin transaction;");
		stm.step();

		stm.create(db(), L"select c.id, t.status, t.description, t.time from tempLogs t, contacts c where t.id like c.contactId and t.netClass like c.netClass;");

		while(true) {
			int rc = stm.step();
			if(rc == SQLITE_ROW) {
				try {
					SQLStatement us;
					us.create(db(), L"insert into logs(contactID, status, description, time) values(?, ?, ?, ?);");
					us.bind(1, stm.getString(0));
					us.bind(2, stm.getString(1));
					us.bind(3, stm.getString(2));
					us.bind(4, stm.getString(3));
					us.step();
				} catch(const SQLException& /*ex*/) {
					//__LOG_F(wtw, 1, L"STLG", L"Failed to select values from DB: %s", ex.message());
				}
				continue;
			} 

			break;
		}

		stm.create(db(), L"commit;");
		stm.step();

		stm.create(db(), L"delete from tempLogs;");
		stm.step();
	} catch(const SQLException& /*ex*/) {
		//__LOG_F(wtw, 1, L"STLG", L"Failed to select values from DB: %s", ex.message());
	}
}

void Logger::onStatus(wtwPresenceDef* prs) {
	if(prs->curStatus < 1 || prs->curStatus > 9)
		return;

	std::wstring name;

	void* h = 0;
	
	wtw->fnCall(WTW_CTL_CONTACT_FIND_EX, (WTW_PARAM)prs->pContactData, (WTW_PARAM)&h);
	
	if(h) {
		wtwContactListItem i;
		initStruct(i);
		wtw->fnCall(WTW_CTL_CONTACT_GET, (WTW_PARAM)h, (WTW_PARAM)&i);
		name = i.itemText;
	} else {
		name = prs->pContactData->id;
	}

	try {
		SQLStatement st;
		st.create(db.database(), L"insert into tempLogs(id, netClass, name, status, description, netId) values(?, ?, ?, ?, ?, ?);");
		st.bind(1, prs->pContactData->id);
		st.bind(2, prs->pContactData->netClass);
		st.bind(3, name.c_str());
		st.bind(4, prs->curStatus);
		if(wcslen(prs->curDescription) > 0) {
			st.bind(5, prs->curDescription);
		} else {
			st.bind(5);
		}
		st.bind(6, prs->pContactData->netId);

		st.step();
	} catch(const SQLException& /*ex*/) {
		//__LOG_F(wtw, 1, L"STLG", L"Failed to insert values into SQL table: %s", ex.message());
	}
}

WTW_PTR Logger::dispatchProtocol(WTW_PARAM lParam, WTW_PARAM wParam, void* data) {
	Logger* parent = reinterpret_cast<Logger*>(data);
	if(!parent)
		return 0;

	wtwProtocolEvent* evn = (wtwProtocolEvent*)lParam;

	if(evn->type == WTW_PEV_TYPE_AFTER && evn->event == WTW_PEV_PRESENCE_RECV) {
		if(evn->pContactData != 0 && wcscmp(evn->pContactData->netClass, L"META") == 0) {
			return 0;
		}

		parent->onStatus(reinterpret_cast<wtwPresenceDef*>(wParam));
	}
	return 0;
}
