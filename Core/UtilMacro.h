#pragma once

#define STRINGIFY(content) #content

#define CLASS_INFO(child, parent) \
private: \
using Super = parent; \
public: \
constexpr static std::string_view kClassConcreteName = STRINGIFY(child);