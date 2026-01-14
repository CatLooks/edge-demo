#pragma once

/// Easing function type.
typedef float (*Easing)(float);

/// Easing collection namespace.
namespace Easings {
	/// Linear easing function.
	float linear(float x) { return x; };

	/// Sine in linear out easing.
	float sineIn (float x) { return 1.0f - cosf(x * M_PI_2); };
	/// Linear in sine out easing.
	float sineOut(float x) { return sinf(x * M_PI_2); };
	/// Sine in sine out easing.
	float sine   (float x) { return 0.5f * (1.0f - cosf(x * M_PI)); };

	/// Quad in linear out easing.
	float quadIn (float x) { return x * x; };
	/// Linear in quad out easing.
	float quadOut(float x) { return x * (2.0f - x); };
	/// Quad in quad out easing.
	float quad   (float x) { return x < 0.5f ? (2 * x * x) : (1 - 2 * (1 - x) * (1 - x)); };
};