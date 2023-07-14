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

// 3D vector
struct SVector3 : public DirectX::XMFLOAT3
{
    SVector3() noexcept : XMFLOAT3(0.f, 0.f, 0.f)
    {
    }

    constexpr explicit SVector3(float ix) noexcept : XMFLOAT3(ix, ix, ix)
    {
    }

    constexpr SVector3(float ix, float iy, float iz) noexcept : XMFLOAT3(ix, iy, iz)
    {
    }

    explicit SVector3(_In_reads_(3) const float* pArray) noexcept : XMFLOAT3(pArray)
    {
    }

    SVector3(DirectX::FXMVECTOR V) noexcept { XMStoreFloat3(this, V); }

    SVector3(const XMFLOAT3& V) noexcept
    {
        this->x = V.x;
        this->y = V.y;
        this->z = V.z;
    }

    explicit SVector3(const DirectX::XMVECTORF32& F) noexcept
    {
        this->x = F.f[0];
        this->y = F.f[1];
        this->z = F.f[2];
    }

    SVector3(const SVector3&) = default;
    SVector3& operator=(const SVector3&) = default;

    SVector3(SVector3&&) = default;
    SVector3& operator=(SVector3&&) = default;

    operator DirectX::XMVECTOR() const noexcept { return XMLoadFloat3(this); }

    // Access operators
    float operator [](int i) const noexcept
    {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    float& operator [](int i) noexcept
    {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    // Comparison operators
    bool operator ==(const SVector3& V) const noexcept;
    bool operator !=(const SVector3& V) const noexcept;

    // Assignment operators
    SVector3& operator=(const DirectX::XMVECTORF32& F) noexcept
    {
        x = F.f[0];
        y = F.f[1];
        z = F.f[2];
        return *this;
    }

    SVector3& operator+=(const SVector3& V) noexcept;
    SVector3& operator-=(const SVector3& V) noexcept;
    SVector3& operator*=(const SVector3& V) noexcept;
    SVector3& operator*=(float S) noexcept;
    SVector3& operator/=(float S) noexcept;

    // Unary operators
    SVector3 operator+() const noexcept { return *this; }
    SVector3 operator-() const noexcept;

    // Vector operations
    bool InBounds(const SVector3& Bounds) const noexcept;

    float Length() const noexcept;
    float LengthSquared() const noexcept;

    float Dot(const SVector3& V) const noexcept;
    void Cross(const SVector3& V, SVector3& result) const noexcept;
    SVector3 Cross(const SVector3& V) const noexcept;

    void Normalize() noexcept;
    void Normalize(SVector3& result) const noexcept;

    void Clamp(const SVector3& vmin, const SVector3& vmax) noexcept;
    void Clamp(const SVector3& vmin, const SVector3& vmax, SVector3& result) const noexcept;

    // Static functions
    static float Distance(const SVector3& v1, const SVector3& v2) noexcept;
    static float DistanceSquared(const SVector3& v1, const SVector3& v2) noexcept;

    static void Min(const SVector3& v1, const SVector3& v2, SVector3& result) noexcept;
    static SVector3 Min(const SVector3& v1, const SVector3& v2) noexcept;

    static void Max(const SVector3& v1, const SVector3& v2, SVector3& result) noexcept;
    static SVector3 Max(const SVector3& v1, const SVector3& v2) noexcept;

    static void Lerp(const SVector3& v1, const SVector3& v2, float t, SVector3& result) noexcept;
    static SVector3 Lerp(const SVector3& v1, const SVector3& v2, float t) noexcept;

    static void SmoothStep(const SVector3& v1, const SVector3& v2, float t, SVector3& result) noexcept;
    static SVector3 SmoothStep(const SVector3& v1, const SVector3& v2, float t) noexcept;

    static void Barycentric(const SVector3& v1, const SVector3& v2, const SVector3& v3, float f, float g,
                            SVector3& result) noexcept;
    static SVector3 Barycentric(const SVector3& v1, const SVector3& v2, const SVector3& v3, float f, float g) noexcept;

    static void CatmullRom(const SVector3& v1, const SVector3& v2, const SVector3& v3, const SVector3& v4, float t,
                           SVector3& result) noexcept;
    static SVector3 CatmullRom(const SVector3& v1, const SVector3& v2, const SVector3& v3, const SVector3& v4,
                               float t) noexcept;

    static void Hermite(const SVector3& v1, const SVector3& t1, const SVector3& v2, const SVector3& t2, float t,
                        SVector3& result) noexcept;
    static SVector3 Hermite(const SVector3& v1, const SVector3& t1, const SVector3& v2, const SVector3& t2,
                            float t) noexcept;

    static void Reflect(const SVector3& ivec, const SVector3& nvec, SVector3& result) noexcept;
    static SVector3 Reflect(const SVector3& ivec, const SVector3& nvec) noexcept;

    static void Refract(const SVector3& ivec, const SVector3& nvec, float refractionIndex, SVector3& result) noexcept;
    static SVector3 Refract(const SVector3& ivec, const SVector3& nvec, float refractionIndex) noexcept;

    // Constants
    static const SVector3 Zero;
    static const SVector3 One;
    static const SVector3 UnitX;
    static const SVector3 UnitY;
    static const SVector3 UnitZ;
    static const SVector3 Up;
    static const SVector3 Down;
    static const SVector3 Right;
    static const SVector3 Left;
    static const SVector3 Forward;
    static const SVector3 Backward;
};

//------------------------------------------------------------------------------
// Comparision operators
//------------------------------------------------------------------------------

inline bool SVector3::operator ==(const SVector3& V) const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR v2 = XMLoadFloat3(&V);
    return XMVector3Equal(v1, v2);
}

inline bool SVector3::operator !=(const SVector3& V) const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR v2 = XMLoadFloat3(&V);
    return XMVector3NotEqual(v1, v2);
}

//------------------------------------------------------------------------------
// Assignment operators
//------------------------------------------------------------------------------

inline SVector3& SVector3::operator+=(const SVector3& V) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR v2 = XMLoadFloat3(&V);
    XMVECTOR X = XMVectorAdd(v1, v2);
    XMStoreFloat3(this, X);
    return *this;
}

inline SVector3& SVector3::operator-=(const SVector3& V) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR v2 = XMLoadFloat3(&V);
    XMVECTOR X = XMVectorSubtract(v1, v2);
    XMStoreFloat3(this, X);
    return *this;
}

inline SVector3& SVector3::operator*=(const SVector3& V) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR v2 = XMLoadFloat3(&V);
    XMVECTOR X = XMVectorMultiply(v1, v2);
    XMStoreFloat3(this, X);
    return *this;
}

inline SVector3& SVector3::operator*=(float S) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR X = XMVectorScale(v1, S);
    XMStoreFloat3(this, X);
    return *this;
}

inline SVector3& SVector3::operator/=(float S) noexcept
{
    using namespace DirectX;
    assert(S != 0.0f);
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR X = XMVectorScale(v1, 1.f / S);
    XMStoreFloat3(this, X);
    return *this;
}

//------------------------------------------------------------------------------
// Urnary operators
//------------------------------------------------------------------------------

inline SVector3 SVector3::operator-() const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR X = XMVectorNegate(v1);
    SVector3 R;
    XMStoreFloat3(&R, X);
    return R;
}

//------------------------------------------------------------------------------
// Binary operators
//------------------------------------------------------------------------------

inline SVector3 operator+(const SVector3& V1, const SVector3& V2) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(&V1);
    XMVECTOR v2 = XMLoadFloat3(&V2);
    XMVECTOR X = XMVectorAdd(v1, v2);
    SVector3 R;
    XMStoreFloat3(&R, X);
    return R;
}

inline SVector3 operator-(const SVector3& V1, const SVector3& V2) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(&V1);
    XMVECTOR v2 = XMLoadFloat3(&V2);
    XMVECTOR X = XMVectorSubtract(v1, v2);
    SVector3 R;
    XMStoreFloat3(&R, X);
    return R;
}

inline SVector3 operator*(const SVector3& V1, const SVector3& V2) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(&V1);
    XMVECTOR v2 = XMLoadFloat3(&V2);
    XMVECTOR X = XMVectorMultiply(v1, v2);
    SVector3 R;
    XMStoreFloat3(&R, X);
    return R;
}

inline SVector3 operator*(const SVector3& V, float S) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(&V);
    XMVECTOR X = XMVectorScale(v1, S);
    SVector3 R;
    XMStoreFloat3(&R, X);
    return R;
}

inline SVector3 operator/(const SVector3& V1, const SVector3& V2) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(&V1);
    XMVECTOR v2 = XMLoadFloat3(&V2);
    XMVECTOR X = XMVectorDivide(v1, v2);
    SVector3 R;
    XMStoreFloat3(&R, X);
    return R;
}

inline SVector3 operator/(const SVector3& V, float S) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(&V);
    XMVECTOR X = XMVectorScale(v1, 1.f / S);
    SVector3 R;
    XMStoreFloat3(&R, X);
    return R;
}

inline SVector3 operator*(float S, const SVector3& V) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(&V);
    XMVECTOR X = XMVectorScale(v1, S);
    SVector3 R;
    XMStoreFloat3(&R, X);
    return R;
}

//------------------------------------------------------------------------------
// Vector operations
//------------------------------------------------------------------------------

inline bool SVector3::InBounds(const SVector3& Bounds) const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR v2 = XMLoadFloat3(&Bounds);
    return XMVector3InBounds(v1, v2);
}

inline float SVector3::Length() const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR X = XMVector3Length(v1);
    return XMVectorGetX(X);
}

inline float SVector3::LengthSquared() const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR X = XMVector3LengthSq(v1);
    return XMVectorGetX(X);
}

inline float SVector3::Dot(const SVector3& V) const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR v2 = XMLoadFloat3(&V);
    XMVECTOR X = XMVector3Dot(v1, v2);
    return XMVectorGetX(X);
}

inline void SVector3::Cross(const SVector3& V, SVector3& result) const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR v2 = XMLoadFloat3(&V);
    XMVECTOR R = XMVector3Cross(v1, v2);
    XMStoreFloat3(&result, R);
}

inline SVector3 SVector3::Cross(const SVector3& V) const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR v2 = XMLoadFloat3(&V);
    XMVECTOR R = XMVector3Cross(v1, v2);

    SVector3 result;
    XMStoreFloat3(&result, R);
    return result;
}

inline void SVector3::Normalize() noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR X = XMVector3Normalize(v1);
    XMStoreFloat3(this, X);
}

inline void SVector3::Normalize(SVector3& result) const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR X = XMVector3Normalize(v1);
    XMStoreFloat3(&result, X);
}

inline void SVector3::Clamp(const SVector3& vmin, const SVector3& vmax) noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR v2 = XMLoadFloat3(&vmin);
    XMVECTOR v3 = XMLoadFloat3(&vmax);
    XMVECTOR X = XMVectorClamp(v1, v2, v3);
    XMStoreFloat3(this, X);
}

inline void SVector3::Clamp(const SVector3& vmin, const SVector3& vmax, SVector3& result) const noexcept
{
    using namespace DirectX;
    XMVECTOR v1 = XMLoadFloat3(this);
    XMVECTOR v2 = XMLoadFloat3(&vmin);
    XMVECTOR v3 = XMLoadFloat3(&vmax);
    XMVECTOR X = XMVectorClamp(v1, v2, v3);
    XMStoreFloat3(&result, X);
}

//------------------------------------------------------------------------------
// Static functions
//------------------------------------------------------------------------------

inline float SVector3::Distance(const SVector3& v1, const SVector3& v2) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(&v1);
    XMVECTOR x2 = XMLoadFloat3(&v2);
    XMVECTOR V = XMVectorSubtract(x2, x1);
    XMVECTOR X = XMVector3Length(V);
    return XMVectorGetX(X);
}

inline float SVector3::DistanceSquared(const SVector3& v1, const SVector3& v2) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(&v1);
    XMVECTOR x2 = XMLoadFloat3(&v2);
    XMVECTOR V = XMVectorSubtract(x2, x1);
    XMVECTOR X = XMVector3LengthSq(V);
    return XMVectorGetX(X);
}

inline void SVector3::Min(const SVector3& v1, const SVector3& v2, SVector3& result) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(&v1);
    XMVECTOR x2 = XMLoadFloat3(&v2);
    XMVECTOR X = XMVectorMin(x1, x2);
    XMStoreFloat3(&result, X);
}

inline SVector3 SVector3::Min(const SVector3& v1, const SVector3& v2) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(&v1);
    XMVECTOR x2 = XMLoadFloat3(&v2);
    XMVECTOR X = XMVectorMin(x1, x2);

    SVector3 result;
    XMStoreFloat3(&result, X);
    return result;
}

inline void SVector3::Max(const SVector3& v1, const SVector3& v2, SVector3& result) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(&v1);
    XMVECTOR x2 = XMLoadFloat3(&v2);
    XMVECTOR X = XMVectorMax(x1, x2);
    XMStoreFloat3(&result, X);
}

inline SVector3 SVector3::Max(const SVector3& v1, const SVector3& v2) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(&v1);
    XMVECTOR x2 = XMLoadFloat3(&v2);
    XMVECTOR X = XMVectorMax(x1, x2);

    SVector3 result;
    XMStoreFloat3(&result, X);
    return result;
}

inline void SVector3::Lerp(const SVector3& v1, const SVector3& v2, float t, SVector3& result) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(&v1);
    XMVECTOR x2 = XMLoadFloat3(&v2);
    XMVECTOR X = XMVectorLerp(x1, x2, t);
    XMStoreFloat3(&result, X);
}

inline SVector3 SVector3::Lerp(const SVector3& v1, const SVector3& v2, float t) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(&v1);
    XMVECTOR x2 = XMLoadFloat3(&v2);
    XMVECTOR X = XMVectorLerp(x1, x2, t);

    SVector3 result;
    XMStoreFloat3(&result, X);
    return result;
}

inline void SVector3::SmoothStep(const SVector3& v1, const SVector3& v2, float t, SVector3& result) noexcept
{
    using namespace DirectX;
    t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t); // Clamp value to 0 to 1
    t = t * t * (3.f - 2.f * t);
    XMVECTOR x1 = XMLoadFloat3(&v1);
    XMVECTOR x2 = XMLoadFloat3(&v2);
    XMVECTOR X = XMVectorLerp(x1, x2, t);
    XMStoreFloat3(&result, X);
}

inline SVector3 SVector3::SmoothStep(const SVector3& v1, const SVector3& v2, float t) noexcept
{
    using namespace DirectX;
    t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t); // Clamp value to 0 to 1
    t = t * t * (3.f - 2.f * t);
    XMVECTOR x1 = XMLoadFloat3(&v1);
    XMVECTOR x2 = XMLoadFloat3(&v2);
    XMVECTOR X = XMVectorLerp(x1, x2, t);

    SVector3 result;
    XMStoreFloat3(&result, X);
    return result;
}

inline void SVector3::Barycentric(const SVector3& v1, const SVector3& v2, const SVector3& v3, float f, float g,
                                  SVector3& result) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(&v1);
    XMVECTOR x2 = XMLoadFloat3(&v2);
    XMVECTOR x3 = XMLoadFloat3(&v3);
    XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);
    XMStoreFloat3(&result, X);
}

inline SVector3 SVector3::Barycentric(const SVector3& v1, const SVector3& v2, const SVector3& v3, float f,
                                      float g) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(&v1);
    XMVECTOR x2 = XMLoadFloat3(&v2);
    XMVECTOR x3 = XMLoadFloat3(&v3);
    XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);

    SVector3 result;
    XMStoreFloat3(&result, X);
    return result;
}

inline void SVector3::CatmullRom(const SVector3& v1, const SVector3& v2, const SVector3& v3, const SVector3& v4,
                                 float t, SVector3& result) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(&v1);
    XMVECTOR x2 = XMLoadFloat3(&v2);
    XMVECTOR x3 = XMLoadFloat3(&v3);
    XMVECTOR x4 = XMLoadFloat3(&v4);
    XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);
    XMStoreFloat3(&result, X);
}

inline SVector3 SVector3::CatmullRom(const SVector3& v1, const SVector3& v2, const SVector3& v3, const SVector3& v4,
                                     float t) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(&v1);
    XMVECTOR x2 = XMLoadFloat3(&v2);
    XMVECTOR x3 = XMLoadFloat3(&v3);
    XMVECTOR x4 = XMLoadFloat3(&v4);
    XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);

    SVector3 result;
    XMStoreFloat3(&result, X);
    return result;
}

inline void SVector3::Hermite(const SVector3& v1, const SVector3& t1, const SVector3& v2, const SVector3& t2, float t,
                              SVector3& result) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(&v1);
    XMVECTOR x2 = XMLoadFloat3(&t1);
    XMVECTOR x3 = XMLoadFloat3(&v2);
    XMVECTOR x4 = XMLoadFloat3(&t2);
    XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);
    XMStoreFloat3(&result, X);
}

inline SVector3 SVector3::Hermite(const SVector3& v1, const SVector3& t1, const SVector3& v2, const SVector3& t2,
                                  float t) noexcept
{
    using namespace DirectX;
    XMVECTOR x1 = XMLoadFloat3(&v1);
    XMVECTOR x2 = XMLoadFloat3(&t1);
    XMVECTOR x3 = XMLoadFloat3(&v2);
    XMVECTOR x4 = XMLoadFloat3(&t2);
    XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);

    SVector3 result;
    XMStoreFloat3(&result, X);
    return result;
}

inline void SVector3::Reflect(const SVector3& ivec, const SVector3& nvec, SVector3& result) noexcept
{
    using namespace DirectX;
    XMVECTOR i = XMLoadFloat3(&ivec);
    XMVECTOR n = XMLoadFloat3(&nvec);
    XMVECTOR X = XMVector3Reflect(i, n);
    XMStoreFloat3(&result, X);
}

inline SVector3 SVector3::Reflect(const SVector3& ivec, const SVector3& nvec) noexcept
{
    using namespace DirectX;
    XMVECTOR i = XMLoadFloat3(&ivec);
    XMVECTOR n = XMLoadFloat3(&nvec);
    XMVECTOR X = XMVector3Reflect(i, n);

    SVector3 result;
    XMStoreFloat3(&result, X);
    return result;
}

inline void SVector3::Refract(const SVector3& ivec, const SVector3& nvec, float refractionIndex,
                              SVector3& result) noexcept
{
    using namespace DirectX;
    XMVECTOR i = XMLoadFloat3(&ivec);
    XMVECTOR n = XMLoadFloat3(&nvec);
    XMVECTOR X = XMVector3Refract(i, n, refractionIndex);
    XMStoreFloat3(&result, X);
}

inline SVector3 SVector3::Refract(const SVector3& ivec, const SVector3& nvec, float refractionIndex) noexcept
{
    using namespace DirectX;
    XMVECTOR i = XMLoadFloat3(&ivec);
    XMVECTOR n = XMLoadFloat3(&nvec);
    XMVECTOR X = XMVector3Refract(i, n, refractionIndex);

    SVector3 result;
    XMStoreFloat3(&result, X);
    return result;
}
