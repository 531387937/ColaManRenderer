#pragma once

struct SColor
{
public:
    SColor() = default;

    SColor(float Value)
        : R(Value), G(Value), B(Value), A(Value)
    {
    }

    SColor(float InR, float InG, float InB, float InA = 1.0f)
        : R(InR), G(InG), B(InB), A(InA)
    {
    }

public:
    static const SColor Black;
    static const SColor White;
    static const SColor Red;
    static const SColor Green;
    static const SColor Blue;
    static const SColor Yellow;
    static const SColor Cyan;
    static const SColor Magenta;

public:
    float R = 0;
    float G = 0;
    float B = 0;
    float A = 0;
};
