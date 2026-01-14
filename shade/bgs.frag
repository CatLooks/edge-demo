#version 330 core

// piped position data
in vec2 tex;

// fragment color output
layout (location = 0) out vec4 pixel;

// background gradient colors
uniform vec3 light;
uniform vec3 dark;

/// Linear interpolation from a to b.
/// @param a Starting point (t = 0).
/// @param b Ending point (t = 1).
/// @param t Interpolation parameter.
vec3 lerp(vec3 a, vec3 b, float t) {
	return a + (b - a) * t;
};

/// Raises x to power of 2.
float sqr(float x) {
	return x * x;
};

/// Fragment shader code.
void main() {
	// set output color
	pixel = vec4(lerp(light, dark, tex.y + 0.5f + 0.5f * sqr(tex.x)), 1.0f);
}