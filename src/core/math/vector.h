#pragma once

#include "core/types.h"
#include <array>
#include <iostream>

namespace Rtek
{
    class Vector3
    {
    public:
        static constexpr int dimension = 3; //!< Dimension of the vector.

        Vector3() : vec{} {}
        Vector3(f32 x, f32 y, f32 z) : vec{{x, y, z}} {}

        f32 x() const;
        f32 y() const;
        f32 z() const;

        Vector3 const& operator+() const;
        Vector3 operator-() const;
        f32 operator[](size_t i) const;
        f32& operator[](size_t i);

        Vector3& operator+=(Vector3 const& o);
        Vector3& operator-=(Vector3 const& o);
        Vector3& operator*=(Vector3 const& o);
        Vector3& operator/=(Vector3 const& o);
        Vector3& operator*=(f32 c);
        Vector3& operator/=(f32 c);

        f32 length() const;
        f32 squared_length() const;
        void make_unit_vector();
        bool is_nan() const;

        std::string to_string() const;

    private:
        std::array<f32, 3> vec;
    };

    std::istream& operator>>(std::istream& is, Vector3& v);
    std::ostream& operator<<(std::ostream& os, Vector3 const& v);

    bool operator==(Vector3 const& lhs, Vector3 const& rhs);
    bool operator!=(Vector3 const& lhs, Vector3 const& rhs);

    Vector3 operator+(Vector3 const& v1, Vector3 const& v2);
    Vector3 operator-(Vector3 const& v1, Vector3 const& v2);
    Vector3 operator*(Vector3 const& v1, Vector3 const& v2);
    Vector3 operator/(Vector3 const& v1, Vector3 const& v2);

    Vector3 operator*(Vector3 const& v, f32 c);
    Vector3 operator*(f32 c, Vector3 const& v);
    Vector3 operator/(Vector3 const& v, f32 c);

    f32 dot(Vector3 const& v1, Vector3 const& v2);
    Vector3 cross(Vector3 const& v1, Vector3 const& v2);
    Vector3 unit_vector(Vector3 const& v);
}
