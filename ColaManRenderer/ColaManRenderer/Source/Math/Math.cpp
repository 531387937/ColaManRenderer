#include "Math.h"

const SVector3 SVector3::Zero = { 0.f, 0.f, 0.f };
const SVector3 SVector3::One = { 1.f, 1.f, 1.f };
const SVector3 SVector3::UnitX = { 1.f, 0.f, 0.f };
const SVector3 SVector3::UnitY = { 0.f, 1.f, 0.f };
const SVector3 SVector3::UnitZ = { 0.f, 0.f, 1.f };
const SVector3 SVector3::Up = { 0.f, 1.f, 0.f };
const SVector3 SVector3::Down = { 0.f, -1.f, 0.f };
const SVector3 SVector3::Right = { 1.f, 0.f, 0.f };
const SVector3 SVector3::Left = { -1.f, 0.f, 0.f };
const SVector3 SVector3::Forward = { 0.f, 0.f, -1.f };
const SVector3 SVector3::Backward = { 0.f, 0.f, 1.f };

const SVector4 SVector4::Zero = { 0.f, 0.f, 0.f, 0.f };
const SVector4 SVector4::One = { 1.f, 1.f, 1.f, 1.f };
const SVector4 SVector4::UnitX = { 1.f, 0.f, 0.f, 0.f };
const SVector4 SVector4::UnitY = { 0.f, 1.f, 0.f, 0.f };
const SVector4 SVector4::UnitZ = { 0.f, 0.f, 1.f, 0.f };
const SVector4 SVector4::UnitW = { 0.f, 0.f, 0.f, 1.f };

const SMatrix SMatrix::Identity = { 1.f, 0.f, 0.f, 0.f,
								  0.f, 1.f, 0.f, 0.f,
								  0.f, 0.f, 1.f, 0.f,
								  0.f, 0.f, 0.f, 1.f };


const float CMath::Infinity = FLT_MAX;
const float CMath::Pi = 3.1415926535f;