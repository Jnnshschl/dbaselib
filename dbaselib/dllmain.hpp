#pragma once

#include <set>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>

#include "dbase/dBase3.hpp"
#include "helpers/dBase.hpp"
#include "helpers/dBaseUtils.hpp"

inline DBase* dbase = nullptr;

extern "C" __declspec(dllexport) bool __stdcall Load(const char* dbfFilePath) noexcept;
extern "C" __declspec(dllexport) void __stdcall Save(const char* dbfFilePath) noexcept;
extern "C" __declspec(dllexport) void __stdcall Unload() noexcept;

extern "C" __declspec(dllexport) char __stdcall GetFieldType(const char* col) noexcept;

extern "C" __declspec(dllexport) void __stdcall ReplaceColumns(const char* src, const char* dst) noexcept;
extern "C" __declspec(dllexport) void __stdcall AddPercent(const char* col, float percent) noexcept;
extern "C" __declspec(dllexport) void __stdcall InsertText(const char* col, int offset, const char* text) noexcept;
extern "C" __declspec(dllexport) void __stdcall SetDate(int d, int m, int y) noexcept;

constexpr auto GetSameCharCount(const char* a, const char* b, size_t max) noexcept
{
    for (size_t i = 0; i < max; ++i)
    {
        if (*(a + i) != *(b + i)) return i;
    }

    return max;
}