// Modified version of DirectXTK12's source file

//--------------------------------------------------------------------------------------
// File: BinaryReader.h
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#pragma once

#include "PlatformHelpers.h"


// Helper for reading binary data, either from the filesystem a memory buffer.
class TBinaryReader
{
public:
    explicit TBinaryReader(_In_z_ const wchar_t* fileName) noexcept(false);
    TBinaryReader(_In_reads_bytes_(dataSize) const uint8_t* dataBlob, size_t dataSize) noexcept;

    TBinaryReader(const TBinaryReader&) = delete;
    TBinaryReader& operator=(const TBinaryReader&) = delete;

    // Reads a single value.
    template <typename T>
    const T& Read()
    {
        return *ReadArray<T>(1);
    }


    // Reads an array of values.
    template <typename T>
    const T* ReadArray(size_t elementCount)
    {
        static_assert(std::is_pod_v<T>, "Can only read plain-old-data types");

        const uint8_t* newPos = mPos + sizeof(T) * elementCount;

        if (newPos < mPos)
            throw std::overflow_error("ReadArray");

        if (newPos > mEnd)
            throw std::runtime_error("End of file");

        auto result = reinterpret_cast<const T*>(mPos);

        mPos = newPos;

        return result;
    }


    // Lower level helper reads directly from the filesystem into memory.
    static HRESULT ReadEntireFile(_In_z_ const wchar_t* fileName, _Inout_ std::unique_ptr<uint8_t[]>& data,
                                  _Out_ size_t* dataSize);

private:
    // The data currently being read.
    const uint8_t* mPos;
    const uint8_t* mEnd;

    std::unique_ptr<uint8_t[]> mOwnedData;
};
