#pragma once

/// 3D fixed vector.
struct vec3x {
	fix x; // X component.
	fix y; // Y component.
	fix z; // Z component.

	/// Empty constructor.
	/// @note Defaults to 0.
	vec3x (): x(0), y(0), z(0) {};
	/// Default constructor.
	/// @param x X component.
	/// @param y Y component.
	/// @param z Z component.
	vec3x (fix x, fix y, fix z): x(x), y(y), z(z) {};
	/// Single value constructor.
	/// @param c X & Y & Z component.
	vec3x (fix c): x(c), y(c), z(c) {};

	/// Yielding arithmetic operations.
	vec3x operator+(const vec3x& oth) const { return vec3x(x + oth.x, y + oth.y, z + oth.z); };
	vec3x operator-(const vec3x& oth) const { return vec3x(x - oth.x, y - oth.y, z - oth.z); };
	vec3x operator*(const vec3x& oth) const { return vec3x(x * oth.x, y * oth.y, z * oth.z); };
	vec3x operator/(const vec3x& oth) const { return vec3x(x / oth.x, y / oth.y, z / oth.z); };
	vec3x operator%(const vec3x& oth) const { return vec3x(x % oth.x, y % oth.y, z % oth.z); };

	/// Yielding arithmetic operations.
	vec3x operator*(fix scale) const { return vec3x(x * scale, y * scale, z * scale); };
	vec3x operator/(fix scale) const { return vec3x(x / scale, y / scale, z / scale); };

	/// State arithmetic operations.
	vec3x& operator+=(const vec3x& oth) { return *this = *this + oth; };
	vec3x& operator-=(const vec3x& oth) { return *this = *this - oth; };
	vec3x& operator*=(const vec3x& oth) { return *this = *this * oth; };
	vec3x& operator/=(const vec3x& oth) { return *this = *this / oth; };
	vec3x& operator%=(const vec3x& oth) { return *this = *this % oth; };

	/// State arithmetic operations.
	vec3x& operator*=(fix scale) { return *this = *this * scale; };
	vec3x& operator/=(fix scale) { return *this = *this / scale; };

	/// Comparison functions.
	bool operator==(const vec3x& oth) const { return x == oth.x && y == oth.y && z == oth.z; };
	bool operator!=(const vec3x& oth) const { return x != oth.x || y != oth.y || z != oth.z; };

	/// Returns maximum coordinates from 2 vectors.
	static vec3x max(const vec3x& a, const vec3x& b) {
		return vec3x {
			a.x > b.x ? a.x : b.x,
			a.y > b.y ? a.y : b.y,
			a.z > b.z ? a.z : b.z
		};
	};
	/// Returns minimum coordinates from 2 vectors.
	static vec3x min(const vec3x& a, const vec3x& b) {
		return vec3x {
			a.x < b.x ? a.x : b.x,
			a.y < b.y ? a.y : b.y,
			a.z < b.z ? a.z : b.z
		};
	};

	/// Returns square of vector length.
	fix sqr() const { return x * x + y * y + z * z; };

	/// Linearly interpolates between 2 vectors.
	/// @param a Starting vector (t = 0).
	/// @param b Ending vector (t = 1).
	/// @param t Interpolation coefficient.
	static vec3x lerp(vec3x a, vec3x b, fix t) { return vec3x(t.lerp(a.x, b.x), t.lerp(a.y, b.y), t.lerp(a.z, b.z)); };

	/// Returns a GL equivalent vector.
	glm::vec3 gl() const { return glm::vec3((float)x, (float)z, (float)y); };
	/// Returns an equivalent integer vector.
	sf::Vector3i int3() const { return sf::Vector3i((int)x, (int)y, (int)z); };
	/// Returns an equivalent integer vector.
	/// @note Vector is rounded up.
	sf::Vector3i up3() const { return sf::Vector3i((int)x.ceil(), (int)y.ceil(), (int)z.ceil()); };
	/// Creates vector from GL equivalent.
	static vec3x fromGL(glm::vec3 v) { return vec3x(v.x, v.z, v.y); };
	/// Creates a directional normal vector.
	/// @param dir Direction.
	static vec3x fromDir(Direction dir) {
		switch (dir) {
			case East : return vec3x(-1,  0, 0);
			case West : return vec3x( 1,  0, 0);
			case North: return vec3x( 0, -1, 0);
			case South: return vec3x( 0,  1, 0);
		};
		return vec3x(0);
	};
};