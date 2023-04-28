#include "core/math/vector.h"

#include "core/assert.h"
#include <cassert>
#include <cmath>
#include <sstream>

using namespace rk;
using namespace sds;

f32 Vector3::x() const { return vec[0]; }
f32 Vector3::y() const { return vec[1]; }
f32 Vector3::z() const { return vec[2]; }

Vector3 const& Vector3::operator+() const { return *this; }
Vector3 Vector3::operator-() const { return {-vec[0], -vec[1], -vec[2]}; }
f32 Vector3::operator[](size_t i) const { return vec[i]; }
f32& Vector3::operator[](size_t i) { return vec[i]; }

Vector3& Vector3::operator+=(Vector3 const& o)
{
    vec[0] += o[0];
    vec[1] += o[1];
    vec[2] += o[2];
    return *this;
}

Vector3& Vector3::operator-=(Vector3 const& o)
{
    vec[0] -= o[0];
    vec[1] -= o[1];
    vec[2] -= o[2];
    return *this;
}

Vector3& Vector3::operator*=(Vector3 const& o)
{
    vec[0] *= o[0];
    vec[1] *= o[1];
    vec[2] *= o[2];
    return *this;
}
Vector3& Vector3::operator/=(Vector3 const& o)
{
    vec[0] /= o[0];
    vec[1] /= o[1];
    vec[2] /= o[2];
    return *this;
}

Vector3& Vector3::operator*=(f32 const c)
{
    vec[0] *= c;
    vec[1] *= c;
    vec[2] *= c;
    return *this;
}
Vector3& Vector3::operator/=(f32 const c)
{
    vec[0] /= c;
    vec[1] /= c;
    vec[2] /= c;
    return *this;
}

f32 Vector3::length() const
{
    return static_cast<f32>(sqrt(squared_length())); // TODO(sdsmith): cast down
}

f32 Vector3::squared_length() const { return vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]; }

void Vector3::make_unit_vector()
{
    f32 k = 1.0f / squared_length();
    *this *= k;
}

bool Vector3::is_nan() const
{
    return std::isnan(vec[0]) || std::isnan(vec[1]) || std::isnan(vec[2]);
}

std::string Vector3::to_string() const
{
    std::stringstream ss;
    ss << vec[0] << " " << vec[1] << " " << vec[2];
    return ss.str();
}

std::istream& rk::operator>>(std::istream& is, Vector3& v) { return is >> v[0] >> v[1] >> v[2]; }

std::ostream& rk::operator<<(std::ostream& os, Vector3 const& v)
{
    return os << v[0] << " " << v[1] << " " << v[2];
}

bool rk::operator==(Vector3 const& lhs, Vector3 const& rhs)
{
    return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]) && (lhs[2] == rhs[2]);
}

bool rk::operator!=(Vector3 const& lhs, Vector3 const& rhs) { return !(lhs == rhs); }

Vector3 rk::operator+(Vector3 const& v1, Vector3 const& v2)
{
    Vector3 u(v1);
    u += v2;
    return u;
}
Vector3 rk::operator-(Vector3 const& v1, Vector3 const& v2)
{
    Vector3 u(v1);
    u -= v2;
    return u;
}

Vector3 rk::operator*(Vector3 const& v1, Vector3 const& v2)
{
    Vector3 u(v1);
    u *= v2;
    return u;
}

Vector3 rk::operator/(Vector3 const& v1, Vector3 const& v2)
{
    Vector3 u(v1);
    u /= v2;
    return u;
}

Vector3 rk::operator*(Vector3 const& v, f32 const c)
{
    Vector3 u(v);
    u *= c;
    return u;
}

Vector3 rk::operator*(f32 const c, Vector3 const& v) { return v * c; }

Vector3 rk::operator/(Vector3 const& v, f32 const c)
{
    Vector3 u(v);
    u /= c;
    return u;
}

f32 rk::dot(Vector3 const& v1, Vector3 const& v2)
{
    Vector3 const u = v1 * v2;
    return u[0] + u[1] + u[2];
}
Vector3 rk::cross(Vector3 const& v1, Vector3 const& v2)
{
    return {(v1[1] * v2[2] - v1[2] * v2[1]), (-(v1[0] * v2[2] - v1[2] * v2[0])),
            (v1[0] * v2[1] - v1[1] * v2[0])};
}

Vector3 rk::unit_vector(Vector3 const& v)
{
    RK_ASSERT(v.length() > 0);
    return v / v.length();
}
