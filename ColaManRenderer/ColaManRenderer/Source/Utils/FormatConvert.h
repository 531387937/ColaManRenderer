#pragma once

#include <string>

class CFormatConvert
{
public:
    static std::string WStrToStr(const std::wstring& WStr)
    {
        if (WStr.empty())
        {
            return std::string();
        }

        int SizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &WStr[0], static_cast<int>(WStr.size()), nullptr, 0, nullptr,
                                             nullptr);
        std::string Str(SizeNeeded, 0);
        WideCharToMultiByte(CP_UTF8, 0, &WStr[0], static_cast<int>(WStr.size()), &Str[0], SizeNeeded, nullptr, nullptr);
        return Str;
    }

    static std::wstring StrToWStr(const std::string& Str)
    {
        if (Str.empty())
        {
            return std::wstring();
        }

        int SizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &Str[0], static_cast<int>(Str.size()), nullptr, 0);
        std::wstring WStr(SizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, &Str[0], static_cast<int>(Str.size()), &WStr[0], SizeNeeded);
        return WStr;
    }

    static LPCWSTR LPCSTRToLPCWSTR(LPCSTR str)
    {
        int size = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
        if (size == 0)
            return nullptr;

        auto buffer = new WCHAR[size];
        MultiByteToWideChar(CP_ACP, 0, str, -1, buffer, size);

        return buffer;
    }
};
