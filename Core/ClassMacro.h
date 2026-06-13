#pragma once

#define CLASS_TO_STRING(_class) #_class

#define CLASS_INFO(child, parent) \
private: \
using Super = parent; \
public: \
constexpr static std::string_view kClassConcreteName = CLASS_TO_STRING(child);