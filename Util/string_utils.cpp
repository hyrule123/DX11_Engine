#include "Engine/Core/pch.h"
#include "string_utils.h"

#include <cctype>

namespace string_utils
{
	std::wstring GetUpperCase(std::wstring wstr)
	{
		size_t size = wstr.size();
		for (size_t i = 0; i < size; ++i)
		{
			wstr[i] = (wchar_t)std::toupper((int)wstr[i]);
		}

		return wstr;
	}
}

