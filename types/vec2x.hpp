#pragma once

/// 2D fixed vector.
struct vec2x {
	fix x; // X component.
	fix y; // Y component.

	/// Empty constructor.
	/// @note Defaults to 0.
	vec2x (): x(0), y(0) {};

	/// Default constructor.
	/// @param x X component.
	/// @param y Y component.
	vec2x (fix x, fix y): x(x), y(y) {};

	vec2x operator+(const vec2x& oth) const { return vec2x(x + oth.x, y + oth.y); };
	vec2x operator-(const vec2x& oth) const { return vec2x(x - oth.x, y - oth.y); };
	vec2x operator*(const vec2x& oth) const { return vec2x(x * oth.x, y * oth.y); };
	vec2x operator/(const vec2x& oth) const { return vec2x(x / oth.x, y / oth.y); };
	vec2x operator%(const vec2x& oth) const { return vec2x(x % oth.x, y % oth.y); };

	vec2x operator*(fix scale) const { return vec2x(x * scale, y * scale); };
	vec2x operator/(fix scale) const { return vec2x(x / scale, y / scale); };

	vec2x& operator+=(const vec2x& oth) { return *this = *this + oth; };
	vec2x& operator-=(const vec2x& oth) { return *this = *this - oth; };
	vec2x& operator*=(const vec2x& oth) { return *this = *this * oth; };
	vec2x& operator/=(const vec2x& oth) { return *this = *this / oth; };
	vec2x& operator%=(const vec2x& oth) { return *this = *this % oth; };

	vec2x& operator*=(fix scale) { return *this = *this * scale; };
	vec2x& operator/=(fix scale) { return *this = *this / scale; };

	bool operator==(const vec2x& oth) const { return x == oth.x && y == oth.y; };
	bool operator!=(const vec2x& oth) const { return x != oth.x || y != oth.y; };
};