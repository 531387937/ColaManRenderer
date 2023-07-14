// Modified version of DirectXTK12's source file

//-------------------------------------------------------------------------------------
// SimpleMath.h -- Simplified C++ Math wrapper for DirectXMath
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//-------------------------------------------------------------------------------------

#pragma once

#include <DirectXMath.h>

// 4D vector
struct SVector4 : public DirectX::XMFLOAT4
{
    SVector4() noexcept : XMFLOAT4(0.f, 0.f, 0.f, 0.f)
    {
    }

    constexpr explicit SVector4(float ix) noexcept : XMFLOAT4(ix, ix, ix, ix)
    {
    }

    constexpr SVector4(float ix, float iy, float iz, float iw) noexcept : XMFLOAT4(ix, iy, iz, iw)
    {
    }

    explicit SVector4(_In_reads_(4) const float* pArray) noexcept : XMFLOAT4(pArray)
    {
    }

    SVector4(DirectX::FXMVECTOR V) noexcept { XMStoreFloat4(this, V); }

    SVector4(const XMFLOAT4& V) noexcept
    {
        this->x = V.x;
        this->y = V.y;
        this->z = V.z;
        this->w = V.w;
    }

    explicit SVector4(const DirectX::XMVECTORF32& F) noexcept
    {
        this->x = F.f[0];
        this->y = F.f[1];
        this->z = F.f[2];
        this->w = F.f[3];
    }

    SVector4(const SVector4&) = default;
    SVector4& operator=(const SVector4&) = default;

    SVector4(SVector4&&) = default;
    SVector4& operator=(SVector4&&) = default;

    operator DirectX::XMVECTOR() const noexcept { return XMLoadFloat4(this); }

    // Comparison operators
    bool operator ==(const SVector4& V) const noexcept;
    bool operator !=(const SVector4& V) const noexcept;

    // Assignment operators
    SVector4& operator=(const DirectX::XMVECTORF32& F) noexcept
    {
        x = F.f[0];
        y = F.f[1];
        z = F.f[2];
        w = F.f[3];
        return *this;
    }

    SVector4& operator+=(const SVector4& V) noexcept;
    SVector4& operator-=(const SVector4& V) noexcept;
    SVector4& operator*=(const SVector4& V) noexcept;
    SVector4& operator*=(float S) noexcept;
    SVector4& operator/=(float S) noexcept;

    // Unary operators
    SVector4 operator+() const noexcept { return *this; }
    SVector4 operator-() const noexcept;

    // Vector operations
    bool InBounds(const SVector4& Bounds) const noexcept;

    float Length() const noexcept;
    float LengthSquared() const noexcept;

    float Dot(const SVector4& V) const noexcept;
    void Cross(const SVector4& v1, const SVector4& v2, SVector4& result) const noexcept;
    SVector4 Cross(const SVector4& v1, const SVector4& v2) const noexcept;

    void Normalize() noexcept;
    void Normalize(SVector4& result) const noexcept;

    void Clamp(const SVector4& vmin, const SVector4& vmax) noexcept;
    void Clamp(const SVector4& vmin, const SVector4& vmax, SVector4& result) const noexcept;

    // Static functions
    static float Distance(const SVector4& v1, const SVector4& v2) noexcept;
    static float DistanceSquared(const SVector4& v1, const SVector4& v2) noexcept;

    static void Min(const SVector4& v1, const SVector4& v2, SVector4& result) noexcept;
    static SVector4 Min(const SVector4& v1, const SVector4& v2) noexcept;

    static void Max(const SVector4& v1, const SVector4& v2, SVector4& result) noexcept;
    static SVector4 Max(const SVector4& v1, const SVector4& v2) noexcept;

    static void Lerp(const SVector4& v1, const SVector4& v2, float t, SVector4& result) noexcept;
    static SVector4 Lerp(const SVector4& v1, const SVector4& v2, float t) noexcept;

    static void SmoothStep(const SVector4& v1, const SVector4& v2, float t, SVector4& result) noexcept;
    static SVector4 SmoothStep(const SVector4& v1, const SVector4& v2, float t) noexcept;

    static void Barycentric(const SVector4& v1, const SVector4& v2, const SVector4& v3, float f, float g,
                            SVector4& result) noexcept;
    static SVector4 Barycentric(const SVector4& v1, const SVector4& v2, const SVector4& v3, float f, float g) noexcept;

    static void CatmullRom(const SVector4& v1, const SVector4& v2, const SVector4& v3, const SVector4& v4, float t,
                           SVector4& result) noexcept;
    static SVector4 CatmullRom(const SVector4& v1, const SVector4& v2, const SVector4& v3, const SVector4& v4,
                               float t) noexcept;

    static void Hermite(const SVector4& v1, const SVector4& t1, const SVector4& v2, const SVector4& t2, float t,
                        SVector4& result) noexcept;
    static SVector4 Hermite(const SVector4& v1, const SVector4& t1, const SVector4& v2, const SVector4& t2,
                            float t) noexcept;

    static void Reflect(const SVector4& ivec, const SVector4& nvec, SVector4& result) noexcept;
    static SVector4 Reflect(const SVector4& ivec, const SVector4& nvec) noexcept;

    static void Refract(const SVector4& ivec, const SVector4& nvec, float refractionIndex, SVector4& result) noexcept;
    static SVector4 Refract(const SVector4& ivec, const SVector4& nvec, float refractionIndex) noexcept;

    // Constants
    static const SVector4 Zero;
    static const SVector4 One;
    static const SVector4 UnitX;
    static const SVector4 UnitY;
    static const SVector4 UnitZ;
    static const SVector4 UnitW;
};


//------------------------------------------------------------------------------
// Comparision operators
//------------------------------------------------------------------------------

inline bool SVector4::operator ==(const SVector4& V) const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(this);
    XMVECTOR v2 = XMLoadFloat4(&V);
    return XMVector4Equal(v1, v2);
}

inline bool SVector4::operator !=(const SVector4& V) const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(this);
    XMVECTOR v2 = XMLoadFloat4(&V);
    return XMVector4NotEqual(v1, v2);
}

//------------------------------------------------------------------------------
// Assignment operators
//------------------------------------------------------------------------------

inline SVector4& SVector4::operator+=(const SVector4& V) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(this);
    XMVECTOR v2 = XMLoadFloat4(&V);
    XMVECTOR X = XMVectorAdd(v1, v2);
    XMStoreFloat4(this, X);
    return *this;
}

inline SVector4& SVector4::operator-=(const SVector4& V) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(this);
    XMVECTOR v2 = XMLoadFloat4(&V);
    XMVECTOR X = XMVectorSubtract(v1, v2);
    XMStoreFloat4(this, X);
    return *this;
}

inline SVector4& SVector4::operator*=(const SVector4& V) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(this);
    XMVECTOR v2 = XMLoadFloat4(&V);
    XMVECTOR X = XMVectorMultiply(v1, v2);
    XMStoreFloat4(this, X);
    return *this;
}

inline SVector4& SVector4::operator*=(float S) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(this);
    XMVECTOR X = XMVectorScale(v1, S);
    XMStoreFloat4(this, X);
    return *this;
}

inline SVector4& SVector4::operator/=(float S) noexcept
{
    using namespace DirectX;
    assert(S != 0.0f);
    XMVECTOR v1 = XMLoadFloat4(this);
    XMVECTOR X = XMVectorScale(v1, 1.f / S);
    XMStoreFloat4(this, X);
    return *this;
}

//------------------------------------------------------------------------------
// Urnary operators
//------------------------------------------------------------------------------

inline SVector4 SVector4::operator-() const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(this);
    XMVECTOR X = XMVectorNegate(v1);
    SVector4 R;
    XMStoreFloat4(&R, X);
    return R;
}

//------------------------------------------------------------------------------
// Binary operators
//------------------------------------------------------------------------------

inline SVector4 operator+(const SVector4& V1, const SVector4& V2) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(&V1);
    XMVECTOR v2 = XMLoadFloat4(&V2);
    XMVECTOR X = XMVectorAdd(v1, v2);
    SVector4 R;
    XMStoreFloat4(&R, X);
    return R;
}

inline SVector4 operator-(const SVector4& V1, const SVector4& V2) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(&V1);
    XMVECTOR v2 = XMLoadFloat4(&V2);
    XMVECTOR X = XMVectorSubtract(v1, v2);
    SVector4 R;
    XMStoreFloat4(&R, X);
    return R;
}

inline SVector4 operator*(const SVector4& V1, const SVector4& V2) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(&V1);
    XMVECTOR v2 = XMLoadFloat4(&V2);
    XMVECTOR X = XMVectorMultiply(v1, v2);
    SVector4 R;
    XMStoreFloat4(&R, X);
    return R;
}

inline SVector4 operator*(const SVector4& V, float S) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(&V);
    XMVECTOR X = XMVectorScale(v1, S);
    SVector4 R;
    XMStoreFloat4(&R, X);
    return R;
}

inline SVector4 operator/(const SVector4& V1, const SVector4& V2) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(&V1);
    XMVECTOR v2 = XMLoadFloat4(&V2);
    XMVECTOR X = XMVectorDivide(v1, v2);
    SVector4 R;
    XMStoreFloat4(&R, X);
    return R;
}

inline SVector4 operator/(const SVector4& V, float S) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(&V);
    XMVECTOR X = XMVectorScale(v1, 1.f / S);
    SVector4 R;
    XMStoreFloat4(&R, X);
    return R;
}

inline SVector4 operator*(float S, const SVector4& V) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(&V);
    XMVECTOR X = XMVectorScale(v1, S);
    SVector4 R;
    XMStoreFloat4(&R, X);
    return R;
}

//------------------------------------------------------------------------------
// Vector operations
//------------------------------------------------------------------------------

inline bool SVector4::InBounds(const SVector4& Bounds) const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(this);
    XMVECTOR v2 = XMLoadFloat4(&Bounds);
    return XMVector4InBounds(v1, v2);
}

inline float SVector4::Length() const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(this);
    XMVECTOR X = XMVector4Length(v1);
    return XMVectorGetX(X);
}

inline float SVector4::LengthSquared() const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(this);
    XMVECTOR X = XMVector4LengthSq(v1);
    return XMVectorGetX(X);
}

inline float SVector4::Dot(const SVector4& V) const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(this);
    XMVECTOR v2 = XMLoadFloat4(&V);
    XMVECTOR X = XMVector4Dot(v1, v2);
    return XMVectorGetX(X);
}

inline void SVector4::Cross(const SVector4& v1, const SVector4& v2, SVector4& result) const noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(this);
    XMVECTOR x2 = XMLoadFloat4(&v1);
    XMVECTOR x3 = XMLoadFloat4(&v2);
    XMVECTOR R = XMVector4Cross(x1, x2, x3);
    XMStoreFloat4(&result, R);
}

inline SVector4 SVector4::Cross(const SVector4& v1, const SVector4& v2) const noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(this);
    XMVECTOR x2 = XMLoadFloat4(&v1);
    XMVECTOR x3 = XMLoadFloat4(&v2);
    XMVECTOR R = XMVector4Cross(x1, x2, x3);

    SVector4 result;
    XMStoreFloat4(&result, R);
    return result;
}

inline void SVector4::Normalize() noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(this);
    XMVECTOR X = XMVector4Normalize(v1);
    XMStoreFloat4(this, X);
}

inline void SVector4::Normalize(SVector4& result) const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(this);
    XMVECTOR X = XMVector4Normalize(v1);
    XMStoreFloat4(&result, X);
}

inline void SVector4::Clamp(const SVector4& vmin, const SVector4& vmax) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(this);
    XMVECTOR v2 = XMLoadFloat4(&vmin);
    XMVECTOR v3 = XMLoadFloat4(&vmax);
    XMVECTOR X = XMVectorClamp(v1, v2, v3);
    XMStoreFloat4(this, X);
}

inline void SVector4::Clamp(const SVector4& vmin, const SVector4& vmax, SVector4& result) const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat4(this);
    XMVECTOR v2 = XMLoadFloat4(&vmin);
    XMVECTOR v3 = XMLoadFloat4(&vmax);
    XMVECTOR X = XMVectorClamp(v1, v2, v3);
    XMStoreFloat4(&result, X);
}

//------------------------------------------------------------------------------
// Static functions
//------------------------------------------------------------------------------

inline float SVector4::Distance(const SVector4& v1, const SVector4& v2) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(&v1);
    XMVECTOR x2 = XMLoadFloat4(&v2);
    XMVECTOR V = XMVectorSubtract(x2, x1);
    XMVECTOR X = XMVector4Length(V);
    return XMVectorGetX(X);
}

inline float SVector4::DistanceSquared(const SVector4& v1, const SVector4& v2) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(&v1);
    XMVECTOR x2 = XMLoadFloat4(&v2);
    XMVECTOR V = XMVectorSubtract(x2, x1);
    XMVECTOR X = XMVector4LengthSq(V);
    return XMVectorGetX(X);
}

inline void SVector4::Min(const SVector4& v1, const SVector4& v2, SVector4& result) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(&v1);
    XMVECTOR x2 = XMLoadFloat4(&v2);
    XMVECTOR X = XMVectorMin(x1, x2);
    XMStoreFloat4(&result, X);
}

inline SVector4 SVector4::Min(const SVector4& v1, const SVector4& v2) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(&v1);
    XMVECTOR x2 = XMLoadFloat4(&v2);
    XMVECTOR X = XMVectorMin(x1, x2);

    SVector4 result;
    XMStoreFloat4(&result, X);
    return result;
}

inline void SVector4::Max(const SVector4& v1, const SVector4& v2, SVector4& result) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(&v1);
    XMVECTOR x2 = XMLoadFloat4(&v2);
    XMVECTOR X = XMVectorMax(x1, x2);
    XMStoreFloat4(&result, X);
}

inline SVector4 SVector4::Max(const SVector4& v1, const SVector4& v2) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(&v1);
    XMVECTOR x2 = XMLoadFloat4(&v2);
    XMVECTOR X = XMVectorMax(x1, x2);

    SVector4 result;
    XMStoreFloat4(&result, X);
    return result;
}

inline void SVector4::Lerp(const SVector4& v1, const SVector4& v2, float t, SVector4& result) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(&v1);
    XMVECTOR x2 = XMLoadFloat4(&v2);
    XMVECTOR X = XMVectorLerp(x1, x2, t);
    XMStoreFloat4(&result, X);
}

inline SVector4 SVector4::Lerp(const SVector4& v1, const SVector4& v2, float t) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(&v1);
    XMVECTOR x2 = XMLoadFloat4(&v2);
    XMVECTOR X = XMVectorLerp(x1, x2, t);

    SVector4 result;
    XMStoreFloat4(&result, X);
    return result;
}

inline void SVector4::SmoothStep(const SVector4& v1, const SVector4& v2, float t, SVector4& result) noexcept
{
    using namespace DirectX;
    t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t); // Clamp value to 0 to 1
    t = t * t * (3.f - 2.f * t);
    XMVECTOR x1 = XMLoadFloat4(&v1);
    XMVECTOR x2 = XMLoadFloat4(&v2);
    XMVECTOR X = XMVectorLerp(x1, x2, t);
    XMStoreFloat4(&result, X);
}

inline SVector4 SVector4::SmoothStep(const SVector4& v1, const SVector4& v2, float t) noexcept
{
    using namespace DirectX;
    t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t); // Clamp value to 0 to 1
    t = t * t * (3.f - 2.f * t);
    XMVECTOR x1 = XMLoadFloat4(&v1);
    XMVECTOR x2 = XMLoadFloat4(&v2);
    XMVECTOR X = XMVectorLerp(x1, x2, t);

    SVector4 result;
    XMStoreFloat4(&result, X);
    return result;
}

inline void SVector4::Barycentric(const SVector4& v1, const SVector4& v2, const SVector4& v3, float f, float g,
                                  SVector4& result) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(&v1);
    XMVECTOR x2 = XMLoadFloat4(&v2);
    XMVECTOR x3 = XMLoadFloat4(&v3);
    XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);
    XMStoreFloat4(&result, X);
}

inline SVector4 SVector4::Barycentric(const SVector4& v1, const SVector4& v2, const SVector4& v3, float f,
                                      float g) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(&v1);
    XMVECTOR x2 = XMLoadFloat4(&v2);
    XMVECTOR x3 = XMLoadFloat4(&v3);
    XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);

    SVector4 result;
    XMStoreFloat4(&result, X);
    return result;
}

inline void SVector4::CatmullRom(const SVector4& v1, const SVector4& v2, const SVector4& v3, const SVector4& v4,
                                 float t, SVector4& result) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(&v1);
    XMVECTOR x2 = XMLoadFloat4(&v2);
    XMVECTOR x3 = XMLoadFloat4(&v3);
    XMVECTOR x4 = XMLoadFloat4(&v4);
    XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);
    XMStoreFloat4(&result, X);
}

inline SVector4 SVector4::CatmullRom(const SVector4& v1, const SVector4& v2, const SVector4& v3, const SVector4& v4,
                                     float t) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(&v1);
    XMVECTOR x2 = XMLoadFloat4(&v2);
    XMVECTOR x3 = XMLoadFloat4(&v3);
    XMVECTOR x4 = XMLoadFloat4(&v4);
    XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);

    SVector4 result;
    XMStoreFloat4(&result, X);
    return result;
}

inline void SVector4::Hermite(const SVector4& v1, const SVector4& t1, const SVector4& v2, const SVector4& t2, float t,
                              SVector4& result) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(&v1);
    XMVECTOR x2 = XMLoadFloat4(&t1);
    XMVECTOR x3 = XMLoadFloat4(&v2);
    XMVECTOR x4 = XMLoadFloat4(&t2);
    XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);
    XMStoreFloat4(&result, X);
}

inline SVector4 SVector4::Hermite(const SVector4& v1, const SVector4& t1, const SVector4& v2, const SVector4& t2,
                                  float t) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat4(&v1);
    XMVECTOR x2 = XMLoadFloat4(&t1);
    XMVECTOR x3 = XMLoadFloat4(&v2);
    XMVECTOR x4 = XMLoadFloat4(&t2);
    XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);

    SVector4 result;
    XMStoreFloat4(&result, X);
    return result;
}

inline void SVector4::Reflect(const SVector4& ivec, const SVector4& nvec, SVector4& result) noexcept
{
    using namespace DirectX;
    XMVECTOR i = XMLoadFloat4(&ivec);
    XMVECTOR n = XMLoadFloat4(&nvec);
    XMVECTOR X = XMVector4Reflect(i, n);
    XMStoreFloat4(&result, X);
}

inline SVector4 SVector4::Reflect(const SVector4& ivec, const SVector4& nvec) noexcept
{
    using namespace DirectX;
    XMVECTOR i = XMLoadFloat4(&ivec);
    XMVECTOR n = XMLoadFloat4(&nvec);
    XMVECTOR X = XMVector4Reflect(i, n);

    SVector4 result;
    XMStoreFloat4(&result, X);
    return result;
}

inline void SVector4::Refract(const SVector4& ivec, const SVector4& nvec, float refractionIndex,
                              SVector4& result) noexcept
{
    using namespace DirectX;
    XMVECTOR i = XMLoadFloat4(&ivec);
    XMVECTOR n = XMLoadFloat4(&nvec);
    XMVECTOR X = XMVector4Refract(i, n, refractionIndex);
    XMStoreFloat4(&result, X);
}

inline SVector4 SVector4::Refract(const SVector4& ivec, const SVector4& nvec, float refractionIndex) noexcept
{
    using namespace DirectX;
    XMVECTOR i = XMLoadFloat4(&ivec);
    XMVECTOR n = XMLoadFloat4(&nvec);
    XMVECTOR X = XMVector4Refract(i, n, refractionIndex);

    SVector4 result;
    XMStoreFloat4(&result, X);
    return result;
}
