/*
** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "wtwPlugin.h"

#define WTW_SCRIPT_STD_LIB		0x00000001
#define WTW_SCRIPT_MATH_LIB		0x00000002
#define WTW_SCRIPT_STRING_LIB	0x00000004
#define WTW_SCRIPT_SYSTEM_LIB	0x00000008
#define WTW_SCRIPT_BLOB_LIB		0x00000010
#define WTW_SCRIPT_API_LIB		0x10000000

/**
 * create new virtual machine instance
 *
 * @params: stackSize, openLibs
 * @return: handle to VM (void*) or NULL
 */
#define WTW_SCRIPT_VM_CREATE		L"ScriptRunner/VMCreate"

/**
 * destroys previously created VM instance
 *
 * @params: handle to VM (void*)
 */
#define WTW_SCRIPT_VM_DESTROY		L"ScriptRunner/VMDestroy"

#define WTW_SCRIPT_TYPE_UNKNOWN			0x00000000
#define WTW_SCRIPT_TYPE_NULL			0x00000001
#define WTW_SCRIPT_TYPE_INTEGER			0x00000002
#define WTW_SCRIPT_TYPE_FLOAT			0x00000004
#define WTW_SCRIPT_TYPE_BOOL			0x00000008
#define WTW_SCRIPT_TYPE_STRING			0x00000010
#define WTW_SCRIPT_TYPE_USERDATA		0x00000080		// use with caution, userpointer is probably better choice in most cases
#define WTW_SCRIPT_TYPE_USERPOINTER		0x00000800

struct wtwScriptArgument
{
	__wtwStdStructDefs(wtwScriptArgument);
	int		structSize;

	QWORD	data;
	int		length;
	int		type;
};

struct wtwScriptFunctionArguments
{
	__wtwStdStructDefs(wtwScriptFunctionArguments);
	int					structSize;

	int					count;
	wtwScriptArgument*	arguments;

	int (*setReturnValue)(void*, wtwScriptArgument*, int);	// vm, argv, argc
	int numReturnArguments;
};

struct wtwScriptExecute
{
	__wtwStdStructDefs(wtwScriptExecute);
	int					structSize;

	const wchar_t*		entryPoint;		// main function

	const wchar_t*		buffer;
	int					bufferSize;
	DWORD				flags;

	wtwScriptArgument*	argv;
	int					argc;

	wtwScriptArgument	returnValue;
};

#define WTW_SCRIPT_EXEC_FLAG_BUFFER_AS_STRING	0x00000001
#define WTW_SCRIPT_EXEC_FLAG_BUFFER_AS_PATH		0x00000002
#define WTW_SCRIPT_EXEC_FLAG_WANT_RETURN_VALUE	0x00000004

/**
 * executes provided buffer or loads script from file
 *
 * @params: handle to VM (void*), wtwScriptExecute*
 * @return: S_OK/S_FALSE
 */
#define WTW_SCRIPT_EXECUTE			L"ScriptRunner/Execute"

struct wtwScriptFunction
{
	__wtwStdStructDefs(wtwScriptFunction);
	int				structSize;

	const wchar_t*	functionName;

	WTWFUNCTION		callback;		// wParam - handle to VM (void*), lParam - wtwScriptFunctionArguments*
	void*			cbData;
};

/**
 * register new function in provided VM instance
 *
 * @params: handle to VM (void*), wtwScriptFunction*
 * @return: S_OK/S_FALSE
 */
#define WTW_SCRIPT_FUNCTION_REGISTER	L"ScriptRunner/FuncReg"
