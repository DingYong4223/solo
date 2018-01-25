/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

namespace solo
{
    struct Degrees;

    struct Radians
    {
        explicit Radians(float raw): raw(raw) {}
        Radians(const Degrees &d);

        auto operator=(const float &f) -> Radians&;
        auto operator=(const Radians &r) -> Radians& = default;
        auto operator=(const Degrees &d) -> Radians&;

        auto toRawDegrees() const -> float;
        auto toRawRadians() const -> float { return raw; }

        auto operator+(const Radians &r) const -> Radians { return Radians(raw + r.raw); }
        auto operator+(const Degrees &d) const -> Radians;
        auto operator+=(const Radians &r) -> Radians&;
        auto operator+=(const Degrees &d) -> Radians&;
        auto operator-() const -> Radians { return Radians(-raw); }
        auto operator-(const Radians &r) const -> Radians { return Radians(raw - r.raw); }
        auto operator-(const Degrees &d) const -> Radians;
        auto operator-=(const Radians &r) -> Radians&;
        auto operator-=(const Degrees &d) -> Radians&;
        auto operator*(float f) const -> Radians { return Radians(raw * f); }
        auto operator*(const Radians &f) const -> Radians { return Radians(raw * f.raw); }
        auto operator*=(float f) -> Radians&;
        auto operator/(float f) const -> Radians { return Radians(raw / f); }
        auto operator/=(float f) -> Radians&;

        bool operator<(const Radians &r) const { return raw <  r.raw; }
        bool operator<=(const Radians &r) const { return raw <= r.raw; }
        bool operator>=(const Radians &r) const { return raw >= r.raw; }
        bool operator>(const Radians &r) const { return raw > r.raw; }

    private:
        float raw;
    };

    inline auto Radians::operator=(const float &f) -> Radians&
    {
        raw = f;
        return *this;
    }

    inline auto Radians::operator+=(const Radians &r) -> Radians&
    {
        raw += r.raw;
        return *this;
    }

    inline auto Radians::operator-=(const Radians &r) -> Radians&
    {
        raw -= r.raw;
        return *this;
    }

    inline auto Radians::operator*=(float f) -> Radians&
    {
        raw *= f;
        return *this;
    }

    inline auto Radians::operator/=(float f) -> Radians&
    {
        raw /= f;
        return *this;
    }
}