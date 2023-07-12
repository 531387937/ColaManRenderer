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

// 2D vector
struct SVector2 : public DirectX::XMFLOAT2
{
	SVector2() noexcept : XMFLOAT2(0.f, 0.f) {}
	constexpr explicit SVector2(float ix) noexcept : XMFLOAT2(ix, ix) {}
	constexpr SVector2(float ix, float iy) noexcept : XMFLOAT2(ix, iy) {}
	explicit SVector2(_In_reads_(2) const float* pArray) noexcept : XMFLOAT2(pArray) {}
	SVector2(DirectX::FXMVECTOR V) noexcept { XMStoreFloat2(this, V); }
	SVector2(const XMFLOAT2& V) noexcept { this->x = V.x; this->y = V.y; }
	explicit SVector2(const DirectX::XMVECTORF32& F) noexcept { this->x = F.f[0]; this->y = F.f[1]; }

	SVector2(const SVector2&) = default;
	SVector2& operator=(const SVector2&) = default;

	SVector2(SVector2&&) = default;
	SVector2& operator=(SVector2&&) = default;

	operator DirectX::XMVECTOR() const noexcept { return XMLoadFloat2(this); }

	// Comparison operators
	bool operator == (const SVector2& V) const noexcept;
	bool operator != (const SVector2& V) const noexcept;

	// Assignment operators
	SVector2& operator= (const DirectX::XMVECTORF32& F) noexcept { x = F.f[0]; y = F.f[1]; return *this; }
	SVector2& operator+= (const SVector2& V) noexcept;
	SVector2& operator-= (const SVector2& V) noexcept;
	SVector2& operator*= (const SVector2& V) noexcept;
	SVector2& operator*= (float S) noexcept;
	SVector2& operator/= (float S) noexcept;

	// Unary operators
	SVector2 operator+ () const noexcept { return *this; }
	SVector2 operator- () const noexcept { return SVector2(-x, -y); }

	// Vector operations
	bool InBounds(const SVector2& Bounds) const noexcept;

	float Length() const noexcept;
	float LengthSquared() const noexcept;

	float Dot(const SVector2& V) const noexcept;
	void Cross(const SVector2& V, SVector2& result) const noexcept;
	SVector2 Cross(const SVector2& V) const noexcept;

	void Normalize() noexcept;
	void Normalize(SVector2& result) const noexcept;

	void Clamp(const SVector2& vmin, const SVector2& vmax) noexcept;
	void Clamp(const SVector2& vmin, const SVector2& vmax, SVector2& result) const noexcept;

	// Static functions
	static float Distance(const SVector2& v1, const SVector2& v2) noexcept;
	static float DistanceSquared(const SVector2& v1, const SVector2& v2) noexcept;

	static void Min(const SVector2& v1, const SVector2& v2, SVector2& result) noexcept;
	static SVector2 Min(const SVector2& v1, const SVector2& v2) noexcept;

	static void Max(const SVector2& v1, const SVector2& v2, SVector2& result) noexcept;
	static SVector2 Max(const SVector2& v1, const SVector2& v2) noexcept;

	static void Lerp(const SVector2& v1, const SVector2& v2, float t, SVector2& result) noexcept;
	static SVector2 Lerp(const SVector2& v1, const SVector2& v2, float t) noexcept;

	static void SmoothStep(const SVector2& v1, const SVector2& v2, float t, SVector2& result) noexcept;
	static SVector2 SmoothStep(const SVector2& v1, const SVector2& v2, float t) noexcept;

	static void Barycentric(const SVector2& v1, const SVector2& v2, const SVector2& v3, float f, float g, SVector2& result) noexcept;
	static SVector2 Barycentric(const SVector2& v1, const SVector2& v2, const SVector2& v3, float f, float g) noexcept;

	static void CatmullRom(const SVector2& v1, const SVector2& v2, const SVector2& v3, const SVector2& v4, float t, SVector2& result) noexcept;
	static SVector2 CatmullRom(const SVector2& v1, const SVector2& v2, const SVector2& v3, const SVector2& v4, float t) noexcept;

	static void Hermite(const SVector2& v1, const SVector2& t1, const SVector2& v2, const SVector2& t2, float t, SVector2& result) noexcept;
	static SVector2 Hermite(const SVector2& v1, const SVector2& t1, const SVector2& v2, const SVector2& t2, float t) noexcept;

	static void Reflect(const SVector2& ivec, const SVector2& nvec, SVector2& result) noexcept;
	static SVector2 Reflect(const SVector2& ivec, const SVector2& nvec) noexcept;

	static void Refract(const SVector2& ivec, const SVector2& nvec, float refractionIndex, SVector2& result) noexcept;
	static SVector2 Refract(const SVector2& ivec, const SVector2& nvec, float refractionIndex) noexcept;

	// Constants
	static const SVector2 Zero;
	static const SVector2 One;
	static const SVector2 UnitX;
	static const SVector2 UnitY;
};

//------------------------------------------------------------------------------
// Comparision operators
//------------------------------------------------------------------------------

inline bool SVector2::operator == (const SVector2& V) const noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR v2 = XMLoadFloat2(&V);
	return XMVector2Equal(v1, v2);
}

inline bool SVector2::operator != (const SVector2& V) const noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR v2 = XMLoadFloat2(&V);
	return XMVector2NotEqual(v1, v2);
}

//------------------------------------------------------------------------------
// Assignment operators
//------------------------------------------------------------------------------

inline SVector2& SVector2::operator+= (const SVector2& V) noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR v2 = XMLoadFloat2(&V);
	XMVECTOR X = XMVectorAdd(v1, v2);
	XMStoreFloat2(this, X);
	return *this;
}

inline SVector2& SVector2::operator-= (const SVector2& V) noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR v2 = XMLoadFloat2(&V);
	XMVECTOR X = XMVectorSubtract(v1, v2);
	XMStoreFloat2(this, X);
	return *this;
}

inline SVector2& SVector2::operator*= (const SVector2& V) noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR v2 = XMLoadFloat2(&V);
	XMVECTOR X = XMVectorMultiply(v1, v2);
	XMStoreFloat2(this, X);
	return *this;
}

inline SVector2& SVector2::operator*= (float S) noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR X = XMVectorScale(v1, S);
	XMStoreFloat2(this, X);
	return *this;
}

inline SVector2& SVector2::operator/= (float S) noexcept
{
	using namespace DirectX;
	assert(S != 0.0f);
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR X = XMVectorScale(v1, 1.f / S);
	XMStoreFloat2(this, X);
	return *this;
}

//------------------------------------------------------------------------------
// Binary operators
//------------------------------------------------------------------------------

inline SVector2 operator+ (const SVector2& V1, const SVector2& V2) noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(&V1);
	XMVECTOR v2 = XMLoadFloat2(&V2);
	XMVECTOR X = XMVectorAdd(v1, v2);
	SVector2 R;
	XMStoreFloat2(&R, X);
	return R;
}

inline SVector2 operator- (const SVector2& V1, const SVector2& V2) noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(&V1);
	XMVECTOR v2 = XMLoadFloat2(&V2);
	XMVECTOR X = XMVectorSubtract(v1, v2);
	SVector2 R;
	XMStoreFloat2(&R, X);
	return R;
}

inline SVector2 operator* (const SVector2& V1, const SVector2& V2) noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(&V1);
	XMVECTOR v2 = XMLoadFloat2(&V2);
	XMVECTOR X = XMVectorMultiply(v1, v2);
	SVector2 R;
	XMStoreFloat2(&R, X);
	return R;
}

inline SVector2 operator* (const SVector2& V, float S) noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(&V);
	XMVECTOR X = XMVectorScale(v1, S);
	SVector2 R;
	XMStoreFloat2(&R, X);
	return R;
}

inline SVector2 operator/ (const SVector2& V1, const SVector2& V2) noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(&V1);
	XMVECTOR v2 = XMLoadFloat2(&V2);
	XMVECTOR X = XMVectorDivide(v1, v2);
	SVector2 R;
	XMStoreFloat2(&R, X);
	return R;
}

inline SVector2 operator/ (const SVector2& V, float S) noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(&V);
	XMVECTOR X = XMVectorScale(v1, 1.f / S);
	SVector2 R;
	XMStoreFloat2(&R, X);
	return R;
}

inline SVector2 operator* (float S, const SVector2& V) noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(&V);
	XMVECTOR X = XMVectorScale(v1, S);
	SVector2 R;
	XMStoreFloat2(&R, X);
	return R;
}

//------------------------------------------------------------------------------
// Vector operations
//------------------------------------------------------------------------------

inline bool SVector2::InBounds(const SVector2& Bounds) const noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR v2 = XMLoadFloat2(&Bounds);
	return XMVector2InBounds(v1, v2);
}

inline float SVector2::Length() const noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR X = XMVector2Length(v1);
	return XMVectorGetX(X);
}

inline float SVector2::LengthSquared() const noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR X = XMVector2LengthSq(v1);
	return XMVectorGetX(X);
}

inline float SVector2::Dot(const SVector2& V) const noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR v2 = XMLoadFloat2(&V);
	XMVECTOR X = XMVector2Dot(v1, v2);
	return XMVectorGetX(X);
}

inline void SVector2::Cross(const SVector2& V, SVector2& result) const noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR v2 = XMLoadFloat2(&V);
	XMVECTOR R = XMVector2Cross(v1, v2);
	XMStoreFloat2(&result, R);
}

inline SVector2 SVector2::Cross(const SVector2& V) const noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR v2 = XMLoadFloat2(&V);
	XMVECTOR R = XMVector2Cross(v1, v2);

	SVector2 result;
	XMStoreFloat2(&result, R);
	return result;
}

inline void SVector2::Normalize() noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR X = XMVector2Normalize(v1);
	XMStoreFloat2(this, X);
}

inline void SVector2::Normalize(SVector2& result) const noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR X = XMVector2Normalize(v1);
	XMStoreFloat2(&result, X);
}

inline void SVector2::Clamp(const SVector2& vmin, const SVector2& vmax) noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR v2 = XMLoadFloat2(&vmin);
	XMVECTOR v3 = XMLoadFloat2(&vmax);
	XMVECTOR X = XMVectorClamp(v1, v2, v3);
	XMStoreFloat2(this, X);
}

inline void SVector2::Clamp(const SVector2& vmin, const SVector2& vmax, SVector2& result) const noexcept
{
	using namespace DirectX;
	XMVECTOR v1 = XMLoadFloat2(this);
	XMVECTOR v2 = XMLoadFloat2(&vmin);
	XMVECTOR v3 = XMLoadFloat2(&vmax);
	XMVECTOR X = XMVectorClamp(v1, v2, v3);
	XMStoreFloat2(&result, X);
}

//------------------------------------------------------------------------------
// Static functions
//------------------------------------------------------------------------------

inline float SVector2::Distance(const SVector2& v1, const SVector2& v2) noexcept
{
	using namespace DirectX;
	XMVECTOR x1 = XMLoadFloat2(&v1);
	XMVECTOR x2 = XMLoadFloat2(&v2);
	XMVECTOR V = XMVectorSubtract(x2, x1);
	XMVECTOR X = XMVector2Length(V);
	return XMVectorGetX(X);
}

inline float SVector2::DistanceSquared(const SVector2& v1, const SVector2& v2) noexcept
{
	using namespace DirectX;
	XMVECTOR x1 = XMLoadFloat2(&v1);
	XMVECTOR x2 = XMLoadFloat2(&v2);
	XMVECTOR V = XMVectorSubtract(x2, x1);
	XMVECTOR X = XMVector2LengthSq(V);
	return XMVectorGetX(X);
}

inline void SVector2::Min(const SVector2& v1, const SVector2& v2, SVector2& result) noexcept
{
	using namespace DirectX;
	XMVECTOR x1 = XMLoadFloat2(&v1);
	XMVECTOR x2 = XMLoadFloat2(&v2);
	XMVECTOR X = XMVectorMin(x1, x2);
	XMStoreFloat2(&result, X);
}

inline SVector2 SVector2::Min(const SVector2& v1, const SVector2& v2) noexcept
{
	using namespace DirectX;
	XMVECTOR x1 = XMLoadFloat2(&v1);
	XMVECTOR x2 = XMLoadFloat2(&v2);
	XMVECTOR X = XMVectorMin(x1, x2);

	SVector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void SVector2::Max(const SVector2& v1, const SVector2& v2, SVector2& result) noexcept
{
	using namespace DirectX;
	XMVECTOR x1 = XMLoadFloat2(&v1);
	XMVECTOR x2 = XMLoadFloat2(&v2);
	XMVECTOR X = XMVectorMax(x1, x2);
	XMStoreFloat2(&result, X);
}

inline SVector2 SVector2::Max(const SVector2& v1, const SVector2& v2) noexcept
{
	using namespace DirectX;
	XMVECTOR x1 = XMLoadFloat2(&v1);
	XMVECTOR x2 = XMLoadFloat2(&v2);
	XMVECTOR X = XMVectorMax(x1, x2);

	SVector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void SVector2::Lerp(const SVector2& v1, const SVector2& v2, float t, SVector2& result) noexcept
{
	using namespace DirectX;
	XMVECTOR x1 = XMLoadFloat2(&v1);
	XMVECTOR x2 = XMLoadFloat2(&v2);
	XMVECTOR X = XMVectorLerp(x1, x2, t);
	XMStoreFloat2(&result, X);
}

inline SVector2 SVector2::Lerp(const SVector2& v1, const SVector2& v2, float t) noexcept
{
	using namespace DirectX;
	XMVECTOR x1 = XMLoadFloat2(&v1);
	XMVECTOR x2 = XMLoadFloat2(&v2);
	XMVECTOR X = XMVectorLerp(x1, x2, t);

	SVector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void SVector2::SmoothStep(const SVector2& v1, const SVector2& v2, float t, SVector2& result) noexcept
{
	using namespace DirectX;
	t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
	t = t * t * (3.f - 2.f * t);
	XMVECTOR x1 = XMLoadFloat2(&v1);
	XMVECTOR x2 = XMLoadFloat2(&v2);
	XMVECTOR X = XMVectorLerp(x1, x2, t);
	XMStoreFloat2(&result, X);
}

inline SVector2 SVector2::SmoothStep(const SVector2& v1, const SVector2& v2, float t) noexcept
{
	using namespace DirectX;
	t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
	t = t * t * (3.f - 2.f * t);
	XMVECTOR x1 = XMLoadFloat2(&v1);
	XMVECTOR x2 = XMLoadFloat2(&v2);
	XMVECTOR X = XMVectorLerp(x1, x2, t);

	SVector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void SVector2::Barycentric(const SVector2& v1, const SVector2& v2, const SVector2& v3, float f, float g, SVector2& result) noexcept
{
	using namespace DirectX;
	XMVECTOR x1 = XMLoadFloat2(&v1);
	XMVECTOR x2 = XMLoadFloat2(&v2);
	XMVECTOR x3 = XMLoadFloat2(&v3);
	XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);
	XMStoreFloat2(&result, X);
}

inline SVector2 SVector2::Barycentric(const SVector2& v1, const SVector2& v2, const SVector2& v3, float f, float g) noexcept
{
	using namespace DirectX;
	XMVECTOR x1 = XMLoadFloat2(&v1);
	XMVECTOR x2 = XMLoadFloat2(&v2);
	XMVECTOR x3 = XMLoadFloat2(&v3);
	XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);

	SVector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void SVector2::CatmullRom(const SVector2& v1, const SVector2& v2, const SVector2& v3, const SVector2& v4, float t, SVector2& result) noexcept
{
	using namespace DirectX;
	XMVECTOR x1 = XMLoadFloat2(&v1);
	XMVECTOR x2 = XMLoadFloat2(&v2);
	XMVECTOR x3 = XMLoadFloat2(&v3);
	XMVECTOR x4 = XMLoadFloat2(&v4);
	XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);
	XMStoreFloat2(&result, X);
}

inline SVector2 SVector2::CatmullRom(const SVector2& v1, const SVector2& v2, const SVector2& v3, const SVector2& v4, float t) noexcept
{
	using namespace DirectX;
	XMVECTOR x1 = XMLoadFloat2(&v1);
	XMVECTOR x2 = XMLoadFloat2(&v2);
	XMVECTOR x3 = XMLoadFloat2(&v3);
	XMVECTOR x4 = XMLoadFloat2(&v4);
	XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);

	SVector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void SVector2::Hermite(const SVector2& v1, const SVector2& t1, const SVector2& v2, const SVector2& t2, float t, SVector2& result) noexcept
{
	using namespace DirectX;
	XMVECTOR x1 = XMLoadFloat2(&v1);
	XMVECTOR x2 = XMLoadFloat2(&t1);
	XMVECTOR x3 = XMLoadFloat2(&v2);
	XMVECTOR x4 = XMLoadFloat2(&t2);
	XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);
	XMStoreFloat2(&result, X);
}

inline SVector2 SVector2::Hermite(const SVector2& v1, const SVector2& t1, const SVector2& v2, const SVector2& t2, float t) noexcept
{
	using namespace DirectX;
	XMVECTOR x1 = XMLoadFloat2(&v1);
	XMVECTOR x2 = XMLoadFloat2(&t1);
	XMVECTOR x3 = XMLoadFloat2(&v2);
	XMVECTOR x4 = XMLoadFloat2(&t2);
	XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);

	SVector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void SVector2::Reflect(const SVector2& ivec, const SVector2& nvec, SVector2& result) noexcept
{
	using namespace DirectX;
	XMVECTOR i = XMLoadFloat2(&ivec);
	XMVECTOR n = XMLoadFloat2(&nvec);
	XMVECTOR X = XMVector2Reflect(i, n);
	XMStoreFloat2(&result, X);
}

inline SVector2 SVector2::Reflect(const SVector2& ivec, const SVector2& nvec) noexcept
{
	using namespace DirectX;
	XMVECTOR i = XMLoadFloat2(&ivec);
	XMVECTOR n = XMLoadFloat2(&nvec);
	XMVECTOR X = XMVector2Reflect(i, n);

	SVector2 result;
	XMStoreFloat2(&result, X);
	return result;
}

inline void SVector2::Refract(const SVector2& ivec, const SVector2& nvec, float refractionIndex, SVector2& result) noexcept
{
	using namespace DirectX;
	XMVECTOR i = XMLoadFloat2(&ivec);
	XMVECTOR n = XMLoadFloat2(&nvec);
	XMVECTOR X = XMVector2Refract(i, n, refractionIndex);
	XMStoreFloat2(&result, X);
}

inline SVector2 SVector2::Refract(const SVector2& ivec, const SVector2& nvec, float refractionIndex) noexcept
{
	using namespace DirectX;
	XMVECTOR i = XMLoadFloat2(&ivec);
	XMVECTOR n = XMLoadFloat2(&nvec);
	XMVECTOR X = XMVector2Refract(i, n, refractionIndex);

	SVector2 result;
	XMStoreFloat2(&result, X);
	return result;
}