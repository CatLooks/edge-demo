#pragma once

/// Type wrapper that disables any default copying & moving.
/// @tparam T Wrapped type.
template <typename T> class NonCopyable {
	public:
	/// Wrapped value.
	T value;

	/// Empty constructor.
	NonCopyable () {};

	/// Default constructor.
	/// @param value Initial value.
	NonCopyable (T value): value(value) {};

	/// Copy constructor.
	/// @param nc Copied object.
	NonCopyable (const NonCopyable& nc) {};

	/// Move constructor.
	/// @param nc Move object.
	NonCopyable (NonCopyable&& nc) {};

	/// Copy assignment.
	/// @param nc Copied object.
	NonCopyable& operator=(const NonCopyable& nc) { return *this; };

	/// Move assignment.
	/// @param nc Moved object.
	NonCopyable& operator=(NonCopyable&& nc) { return *this; };
};