#pragma once

#include <string>
#include <vector>
#include <charconv>
#include <filesystem>
#include <unordered_map>

#include "fast_float/fast_float.h"

#include "dBase.hpp"

#include "../dbase/dBase3.hpp"

class DBase3Handle : public DBaseHandle
{
public:
    const DBase* dBase;
    const size_t FieldOffset;

    const char* FieldName;
    const size_t FieldSize;
    const size_t FieldDecimals;
    const char FieldType;

private:
    const fast_float::parse_options FFOptions{ fast_float::chars_format::general };
    const float FloatFactor;

public:
    DBase3Handle(const DBase* dbase, DBase3FieldDescriptor* descriptor, int fieldOffset)
        : dBase(dbase),
        FieldOffset(fieldOffset),
        FieldName(descriptor->Name),
        FieldSize((size_t)descriptor->Lenght),
        FieldDecimals((size_t)descriptor->Decimals),
        FieldType(descriptor->FieldType),
        FloatFactor(std::powf(10.0f, (float)descriptor->Decimals))
    {}

    constexpr virtual const char* Name() const noexcept override { return FieldName; }
    constexpr virtual size_t Size() const noexcept override { return FieldSize; }
    constexpr virtual size_t Decimals() const noexcept { return FieldDecimals; }
    constexpr virtual char Type() const noexcept { return FieldType; }

    constexpr virtual char* Data(int row) const noexcept override { return dBase->Records[row] + FieldOffset; }

    virtual void Copy(int row, const DBaseHandle* other, int otherRow) const noexcept override
    {
        auto ptr = Data(row);
        memset(ptr, ' ', FieldSize);
        memcpy(ptr, other->Data(otherRow), std::min(FieldSize, other->Size()));
    }

    virtual void CopyRaw(int row, const void* src, size_t size) const noexcept override
    {
        auto ptr = Data(row);
        memset(ptr, ' ', FieldSize);
        memcpy(ptr, src, std::min(FieldSize, size));
    }

    virtual void Insert(int row, int offset, const void* src, size_t size) const noexcept override
    {
        memcpy(Data(row) + offset, src, std::min(FieldSize - offset, size));
    }

    virtual void CopyR(int row, const DBaseHandle* other, int otherRow) const noexcept override
    {
        auto ptr = Data(row);
        const auto size = std::min(FieldSize, other->Size());
        const auto offset = FieldSize - size;

        memset(ptr, ' ', FieldSize);
        memcpy(ptr + offset, other->Data(otherRow), size);
    }

    virtual void CopyRRaw(int row, const void* src, size_t size) const noexcept override
    {
        auto ptr = Data(row);
        size = std::min(FieldSize, size);
        const auto offset = FieldSize - size;

        memset(ptr, ' ', FieldSize);
        memcpy(ptr + offset, src, size);
    }

    virtual void Clear(int row) const noexcept override { memset(Data(row), ' ', FieldSize); }

    constexpr virtual std::string_view GetText(int row) const noexcept override { return std::string_view(Data(row), FieldSize); }

    constexpr virtual void SetText(int row, const char* text) const noexcept override { CopyRaw(row, text, std::min(FieldSize, strlen(text))); }

    constexpr virtual void SetText(int row, const std::string& text) const noexcept override { CopyRaw(row, text.c_str(), std::min(FieldSize, text.size())); }

    constexpr virtual void SetText(int row, const std::string_view& text) const noexcept override { CopyRaw(row, text.data(), std::min(FieldSize, text.size())); }

    constexpr virtual void SetChar(int row, char c) const noexcept override { *Data(row) = c; }

    virtual void ReplaceText(int row, const char* text, const char* newText) const noexcept override
    {
        auto ptr = Data(row);
        std::string newString(ptr, FieldSize);

        const auto needleLen = strlen(text);
        const auto newNeedleLen = strlen(newText);
        size_t position = 0;

        while ((position = newString.find(text, position)) != std::string::npos)
        {
            newString.replace(position, needleLen, newText);
            position += newNeedleLen;
        }

        memset(ptr, ' ', FieldSize);
        memcpy(ptr, newString.c_str(), std::min(FieldSize, newString.length()));
    }

    virtual float GetFloat(int row) const noexcept override
    {
        const auto str = std::string_view(Data(row), FieldSize);
        float result;
        auto r = fast_float::from_chars_advanced(str.data() + str.find_first_not_of(' '), str.data() + FieldSize, result, FFOptions);
        return result;
    }

    virtual void SetFloat(int row, float f) const noexcept override
    {
        constexpr auto MAX_FLOAT_LEN = 32;

        char buffer[MAX_FLOAT_LEN]{ 0 };
        std::to_chars(buffer, buffer + MAX_FLOAT_LEN, std::roundf((f * FloatFactor)) / FloatFactor, std::chars_format::fixed, 2);

        const auto str = std::string_view(buffer);
        const auto size = std::min(FieldSize, str.length());

        auto ptr = Data(row);
        memset(ptr, ' ', FieldSize);
        memcpy(ptr + (FieldSize - size), str.data(), size);
    }

    constexpr virtual int GetInt(int row) const noexcept override
    {
        const auto str = std::string_view(Data(row), FieldSize);
        const char* p = str.data() + str.find_first_not_of(' ');

        int x = 0;
        bool negative = false;

        if (*p == '-')
        {
            negative = true;
            ++p;
        }

        while (*p >= '0' && *p <= '9')
        {
            x = (x * 10) + (*p - '0');
            ++p;
        }

        return negative ? -x : x;
    }

    virtual void SetInt(int row, int i) const noexcept override
    {
        auto ptr = Data(row);
        const auto str = std::to_string(i);
        const auto size = std::min(FieldSize, str.length());

        memset(ptr, ' ', FieldSize);
        memcpy(ptr + (FieldSize - size), str.c_str(), size);
    }

    constexpr virtual void SetDate(int row, tm t) const noexcept override { SetDate(row, t.tm_mday, t.tm_mon + 1, t.tm_year + 1900); }
    constexpr virtual void SetDate(int row, int d, int m, int y) const noexcept override { SetInt(row, d + (m * 100) + (y * 10000)); }
};

class DBase3 : public DBase
{
public:
    const bool HasMemo;
    const DBase3Header* Header;
    std::unordered_map<std::string, DBase3Handle*> Handles;

    DBase3(char* data, size_t size, bool claimData = true, bool hasMemo = false)
        : DBase(data, size, claimData),
        HasMemo(hasMemo),
        Header(reinterpret_cast<DBase3Header*>(data)),
        Handles()
    {}

    ~DBase3()
    {
        // free the handles
        for (const auto& kv : Handles) delete kv.second;
    }

    constexpr virtual char Version() const noexcept override { return Header->Version; }

    virtual bool Load() noexcept override
    {
        char* data = const_cast<char*>(Data);

        // skip the header
        data += sizeof(DBase3Header);

        const char* eof = data + Size;
        int rowSize = 0;

        // read all field descriptors until we reach the terminator
        while (*data != 0xD)
        {
            auto desc = reinterpret_cast<DBase3FieldDescriptor*>(data);
            Handles[desc->Name] = new DBase3Handle(this, desc, rowSize);

            rowSize += desc->Lenght;
            data += sizeof(DBase3FieldDescriptor);

            if (data >= eof)
            {
                // something went wrong or no data is in the db
                return false;
            }
        }

        // skip field descriptor section terminator
        do
        {
            ++data;
        } while (*data != ' ' && *data != '*');

        // add one to include the row start character
        ++rowSize;

        // read every row until end of file
        for (; data < eof; data += rowSize)
        {
            // first character in a row should always be a space (0x20) or asterisk (0x2A)
            // this indicates the deleted state of a row (asterisk is deleted, space is not)
            if (*data == ' ')
            {
                Records.push_back(data + 1);
            }
            else if (*data == '*')
            {
                Deleted.push_back(data + 1);
            }
        }

        // save our field names for later usage
        FieldNames.reserve(Handles.size());
        std::transform(Handles.begin(), Handles.end(), std::back_inserter(FieldNames), [](const auto& kv) { return kv.second->Name(); });

        return true;
    }

    virtual void Save(std::filesystem::path file) const noexcept override
    {
        std::ofstream dbfOutputStream(file, std::ifstream::out | std::ifstream::binary);
        dbfOutputStream.write(Data, Size);
        dbfOutputStream.close();
    }

    virtual inline DBaseHandle* Select(const std::string& col) const noexcept override { return Handles.at(col); }
};