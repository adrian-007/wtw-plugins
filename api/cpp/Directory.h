#pragma once

#include "Core.h"

namespace wtw
{
	class LIBWTW_API CDirectory
	{
	private:
		CDirectory();
		~CDirectory();

	public:
		static QWORD getDirectorySize(const wchar_t *path, int bRecursive, const wchar_t *filter = L"*.*");
		static QWORD deleteDirectory(const wchar_t *path, int bRecursive, const wchar_t *filter = L"*.*");

	};
}