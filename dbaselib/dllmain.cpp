#include "dllmain.hpp"

bool __stdcall Load(const char* dbfFilePath) noexcept
{
    dbase = DBaseUtils::FromFile(dbfFilePath);
    return dbase && dbase->Load();
}

void __stdcall Save(const char* dbfFilePath) noexcept
{
    dbase->Save(dbfFilePath);
}

void __stdcall Unload() noexcept
{
    delete dbase;
}

char __stdcall GetFieldType(const char* col) noexcept
{
    return dbase->Select(col)->Type();
}

void __stdcall ReplaceColumns(const char* src, const char* dst) noexcept
{
    const auto handle = dbase->Select(src);
    const auto handle2 = dbase->Select(dst);

    const auto srcType = handle->Type();
    const auto copyFn = srcType == 'N' || srcType == 'D' ? &DBaseHandle::CopyR : &DBaseHandle::Copy;

    for (size_t i = 0; i < dbase->RecordCount(); ++i)
    {
        (*handle2.*copyFn)(i, handle, i);
    }
}

void __stdcall AddPercent(const char* col, float percent) noexcept
{
    const auto p = (percent / 100.0f) + 1.0f;
    const auto handle = dbase->Select(col);

    for (size_t i = 0; i < dbase->RecordCount(); ++i)
    {
        handle->SetFloat(i, handle->GetFloat(i) * p);
    }
}

void __stdcall InsertText(const char* col, int offset, const char* text) noexcept
{
    const auto handle = dbase->Select(col);
    const auto textLen = strlen(text);

    for (size_t i = 0; i < dbase->RecordCount(); ++i)
    {
        handle->Insert(i, offset, text, textLen);
    }
}

void __stdcall SetDate(int d, int m, int y) noexcept
{
    const auto handle = dbase->Select("DATE");

    for (size_t i = 0; i < dbase->RecordCount(); ++i)
    {
        handle->SetDate(i, d, m, y);
    }
}