#pragma once

#include <string>
#include <filesystem>
#include <Utils/FormatConvert.h>

class TFileHelpers
{
public:
    static bool IsFileExit(const std::wstring& FileName)
    {
        return std::filesystem::exists(FileName);
    }

    static std::wstring EngineDir()
    {
        std::wstring EngineDir = CFormatConvert::StrToWStr(std::string(SOLUTION_DIR)) + L"ColaManRenderer\\";

        return EngineDir;
    }
};
