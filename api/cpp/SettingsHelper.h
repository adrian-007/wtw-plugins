/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "..\wtwPluginSettings.h"
#include "..\wtwPlugin.h"
#include "Core.h"
#include "BaseObject.h"
#include <list>

namespace wtw {

	class LIBWTW_API CSettings : public CBaseObject
	{
		friend class CSettingsBinder;

	protected:
		void * m_pS;

	public:
		CSettings();
		CSettings(void *pSettings);

		int attach(void *pSettings);
		virtual ~CSettings();
		void * detach();
		void destroy();
		int create(const wchar_t *pFileName);
		int read();
		int write();
		int getInt(const wchar_t *pKeyName, int defaultValue);
		LONGLONG getInt64(const wchar_t *pKeyName, __int64 defaultValue);
		int setInt(const wchar_t *pKeyName, int value);
		int setInt64(const wchar_t *pKeyName, __int64 value);
		int getString(const wchar_t *pKeyName, const wchar_t * defaultValue, wchar_t **pRetPtr);
		int setString(const wchar_t *pKeyName, const wchar_t * pRetPtr);
	};

	class LIBWTW_API CSettingsBinder : public CBaseObject
	{
	public:
		CSettingsBinder();
		CSettingsBinder(HWND hOptWindow);
		virtual ~CSettingsBinder();

	protected:
		struct _binding
		{
			const wchar_t * name;
			DWORD		 type;
			DWORD		 control;
			HWND		 hControl;
			__int64		 def;
		};

		void* m_bindings;
		HWND m_hMainWnd;

	public:

		enum 
		{
			CONTROL_TYPE_CHECKBOX	= 0x0000L,
			CONTROL_TYPE_EDIT,
			CONTROL_TYPE_COMBO_EDIT,
			CONTROL_TYPE_COMBO_INDEX,

			_CONTROL_MAX = 0x7FFFFFFFL // force DWORD
		};

		enum 
		{
			SETTING_TYPE_INT = 0x0000L,
			SETTING_TYPE_STRING,

			_SETTING_MAX = 0x7FFFFFFFL // force DWORD
		};

		void setMainHwnd(HWND hOptWindow);

		int bindInt(
			const wchar_t *settingName, int defaultValue,
			HWND controlHwnd, DWORD controlType = CONTROL_TYPE_CHECKBOX
			);
		int bindInt(
			const wchar_t *settingName, int defaultValue,
			int controlId, DWORD controlType = CONTROL_TYPE_CHECKBOX
			);
		int bindString(
			const wchar_t *settingName, const wchar_t * defaultValue,
			int controlId, DWORD controlType = CONTROL_TYPE_CHECKBOX
			);
		int bindString(
			const wchar_t *settingName, const wchar_t * defaultValue,
			HWND controlHwnd, DWORD controlType = CONTROL_TYPE_EDIT
			);
		int settingsToControls(const CSettings &cs);
		int controlsToSettings(const CSettings &cs);
	};
};