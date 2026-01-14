#pragma once

/// Creates a copy of memory.
/// @param source Memory source.
/// @param size Memory size.
/// @return Allocated copy.
template <typename T> T* memdup(const T* source, size_t size) {
	T* mem = (T*)::operator new(size);
	memcpy(mem, source, size);
	return mem;
};

/// Creates a string from a format call.
/// @param str Format string.
/// @param args Format arguments.
/// @return Heap-allocated string.
template <typename... A> char* formatf(const char* str, A... args) {
	size_t size = snprintf(NULL, 0, str, args...) + 1;
	char* result = new char[size];
	snprintf(result, size, str, args...);
	return result;
};

/// Gets amount of cubes in a dimension vector.
/// @param size Dimension vector.
/// @return Volume of vector parallelepiped.
size_t vol3(sf::Vector3i size) {
	return size.x * size.y * size.z;
};

/// Returns a GL vector from integer vector.
template <typename T> glm::vec3 vec3gl(sf::Vector3<T> v) {
	return glm::vec3(v.x, v.z, v.y);
};

/// Returns absolute difference between 2 integers.
constexpr unsigned int udiff(unsigned int a, unsigned int b) {
	return (a < b) ? (b - a) : (a - b);
};

/// Linear interpolation on 2 integers.
/// @param a Starting point (t = 0).
/// @param b Ending point (t = 1).
/// @param t Interpolation parameter.
/// @return Interpolation result.
int lerpi(int a, int b, float t) {
	return a + (b - a) * t;
};

/// Linear interpolation on 2 real numbers.
/// @param a Starting point (t = 0).
/// @param b Ending point (t = 1).
/// @param t Interpolation parameter.
/// @return Interpolation result.
float lerpf(float a, float b, float t) {
	return a + (b - a) * t;
};

/// Linear interpolation on 2 vectors.
/// @param a Starting point (t = 0).
/// @param b Ending point (t = 1).
/// @param t Interpolation parameter.
/// @return Interpolation result.
glm::vec3 lerpv(glm::vec3 a, glm::vec3 b, float t) {
	return a + (b - a) * t;
};

/// Calculates the middle point.
/// @param a Point A.
/// @param b Point B.
/// @return Point in between of A and B.
glm::vec3 midv(glm::vec3 a, glm::vec3 b) {
	return (a + b) * 0.5f;
};

/// Project a 3D vector on a XZ plane.
/// @param v Input vector.
/// @return Vector projection plane coordinates.
glm::vec2 projXZ(glm::vec3 v) {
	return glm::vec2(v.x, v.z);
};

/// Converts an RGB color into a normalized color.
/// @param r R channel.
/// @param g G channel.
/// @param b B channel.
glm::vec3 glcolor(uint8_t r, uint8_t g, uint8_t b) {
	return {
		r / 255.f,
		g / 255.f,
		b / 255.f
	};
};

/// Converts a single number into a normalized color.
/// @param color Color value.
glm::vec3 RGB(unsigned long long color) {
	return glcolor(color >> 16 & 0xFF, color >> 8 & 0xFF, color & 0xFF);
};

/// Returns number's sign.
/// @note If number is 0, 0 will be returned.
int32_t sign(int32_t num) {
	if (num > 0) return 1;
	if (num < 0) return -1;
	return 0;
};

// include type units
#include "types/nocopy.hpp"
#include "types/direction.hpp"
#include "types/easing.hpp"
#include "types/fixed.hpp"
#include "types/vec2x.hpp"
#include "types/vec3x.hpp"
#include "types/vol3x.hpp"