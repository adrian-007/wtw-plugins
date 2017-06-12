/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "..\wtwPlugin.h"
#include "..\wtwOther.h"
#include "Core.h"
#include "File.h"
#include "String.h"

#define JSON_DEFS

typedef enum {
	JSON_OBJECT,
	JSON_ARRAY,
	JSON_STRING,
	JSON_INTEGER,
	JSON_REAL,
	JSON_TRUE,
	JSON_FALSE,
	JSON_NULL
} json_type;

typedef struct {
	json_type type;
	size_t refcount;
} json_t;

namespace wtw
{
	class CJson;
	class CJsonArray;
	class CJsonObject;
	class CJsonString;
	class CJsonInteger;

	class LIBWTW_API CJsonIter
	{
		friend class wtw::CJson;

	protected:
		CJsonIter(void *iter, wtw::CJson *pObj);

		void *m_iter;
		wtw::CJson *m_pObj;

	public:
		~CJsonIter();

	public:
		void release();
		bool next();
		const char *key();
		wtw::CJson *value();
	};

	class LIBWTW_API CJson : public json_t
	{
	public:
		CJson();
		~CJson();

	public:
		bool isArray();
		bool isObject();
		bool isString();
		bool isTrue();
		bool isFalse();
		bool isInteger();
		bool isNumber();
		bool isBoolean();
		bool isReal();

		const char* c_str();
		__int64 c_int();
		bool c_bool();
		double c_real();
		size_t size();

	public:
		wtw::CJsonIter *getIter();

	public:
		CJson * getAt(size_t index);
		CJson * find(const char *name);

	public:
		CJson * operator [] (size_t index);
		CJson * operator [] (const char *obj);
		HRESULT dump(wtw::CString & str); // wchar_t data
		HRESULT dump(wtw::CBuffer & str); // utf-8 data

		void release();

	public:
		static CJson * load(const char*);
		static CJson * load(const wchar_t*);

		static CJsonArray * newArray();
		static CJsonInteger * newInt(__int64 value);
		static CJsonString * newString(const char*);
		static CJsonObject * newObject();

		static void decref(CJson *); // for compatibility

	};

	class LIBWTW_API CJsonObject abstract : public wtw::CJson
	{
	public:
		CJsonObject();
		~CJsonObject();
	
	public:
		HRESULT set(const char *key, CJson *value);
		HRESULT remove(const char *key);
		CJson * get(const char *key);
		size_t  size();

	};

	class LIBWTW_API CJsonArray abstract : public wtw::CJson
	{
	public:
		CJsonArray();
		~CJsonArray();

	public:
		HRESULT append(CJson *pElement);				// append element to array
		HRESULT set(const int index, CJson *value);
		HRESULT remove(const int index);
		CJson * get(const int index);
		size_t  size();
	};

	class LIBWTW_API CJsonString abstract : public wtw::CJson
	{
	public:
		CJsonString();
		~CJsonString();

	public:
		HRESULT set(const char *value);
		HRESULT set(const wchar_t *value);
		const char* get();
	};

	class LIBWTW_API CJsonInteger abstract : public wtw::CJson
	{
	public:
		CJsonInteger();
		~CJsonInteger();

	public:
		HRESULT set(const __int64 value);
		__int64 get();
	};
};