#pragma once

#include <Engine/Util/StringHashTable.h>

#define STRINGIFY(content) #content
#define SAFE_DELETE(_ptr) if (_ptr) { delete _ptr; _ptr = nullptr; }
#define SAFE_DELETE_ARRAY(_ptr_arr) if (_ptr_arr) { delete[] _ptr_arr; _ptr_arr = nullptr; }