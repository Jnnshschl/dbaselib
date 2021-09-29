#pragma once

#include <fstream>
#include <filesystem>

#include "dBase.hpp"
#include "dBase3.hpp"

namespace DBaseUtils
{
    static DBase* FromFile(std::filesystem::path file) noexcept
    {
        const auto size = (size_t)std::filesystem::file_size(file);
        auto data = new char[size];

        std::ifstream dbfStream(file, std::ifstream::in | std::ifstream::binary);
        dbfStream.read(data, size);
        dbfStream.close();

        DBase* dbase = nullptr;

        // check whether we are able to load this file or not
        switch ((unsigned char)*data)
        {
        case 0x3:   // dBase III
            return new DBase3(data, size, true);

        case 0x83:  // dBase III with Memo
            return new DBase3(data, size, true, true);

        case 0x8B:  // dBase IV with Memo
            return new DBase3(data, size, true, true);

        default:
            return nullptr;
        }
    }
}