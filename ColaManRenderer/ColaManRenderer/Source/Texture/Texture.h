#pragma once

#include <string>
#include "Texture/TextureInfo.h"
#include "D3D12/D3D12Texture.h"
#include "D3D12/D3D12RHI.h"

struct STextureResource
{
    STextureInfo TextureInfo;

    std::vector<uint8_t> TextureData;

    std::vector<D3D12_SUBRESOURCE_DATA> InitData;
};

class CTexture
{
public:
    CTexture(const std::string& InName, ETextureType InType, bool InbSRGB, std::wstring InFilePath)
        : Name(InName), Type(InType), FilePath(InFilePath), bSRGB(InbSRGB)
    {
    }

    virtual ~CTexture()
    {
    }

    CTexture(const CTexture& Other) = delete;

    CTexture& operator=(const CTexture& Other) = delete;

public:
    void LoadTextureResourceFromFlie(CD3D12RHI* D3D12RHI);

    void SetTextureResourceDirectly(const STextureInfo& InTextureInfo, const std::vector<uint8_t>& InTextureData,
                                    const D3D12_SUBRESOURCE_DATA& InInitData);

    void CreateTexture(CD3D12RHI* D3D12RHI);

    CD3D12TextureRef GetD3DTexture() { return D3DTexture; }

private:
    static std::wstring GetExtension(std::wstring path);

    void LoadDDSTexture(CD3D12Device* Device);

    void LoadWICTexture(CD3D12Device* Device);

    void LoadHDRTexture(CD3D12Device* Device);

public:
    std::string Name;

    ETextureType Type;

    std::wstring FilePath;

    bool bSRGB = true;

    STextureResource TextureResource;

    CD3D12TextureRef D3DTexture = nullptr;
};

class CTexture2D : public CTexture
{
public:
    CTexture2D(const std::string& InName, bool InbSRGB, std::wstring InFilePath);

    ~CTexture2D() override;
};

class CTextureCube : public CTexture
{
public:
    CTextureCube(const std::string& InName, bool InbSRGB, std::wstring InFilePath);

    ~CTextureCube() override;
};

class CTexture3D : public CTexture
{
public:
    CTexture3D(const std::string& InName, bool InbSRGB, std::wstring InFilePath);

    ~CTexture3D() override;
};
