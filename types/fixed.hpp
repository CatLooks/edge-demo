#pragma once

/// Fixed point number.
class fix {
	protected:
		/// Number value container.
		int64_t val;

		/// Returns multiplication result of 2 fixed point integers.
		/// @param a First number.
		/// @param b Second number.
		static inline int64_t mul(int64_t a, int64_t b) { return ((__int128_t)a * (__int128_t)b) >> 32; };
		/// Returns multiplication result of 2 fixed point integers.
		/// @param a First number.
		/// @param b Second number.
		static inline int64_t div(int64_t a, int64_t b) { return ((__int128_t)a << 32) / (__int128_t)b; };
		/// Returns remainder of division of 2 fixed point integers.
		/// @param a First number.
		/// @param b Second number.
		static inline int64_t mod(int64_t a, int64_t b) { return ((__int128_t)a << 32) % (__int128_t)b; };

	public:
		/// Empty constructor.
		/// @note Defaults to `0.0`.
		fix (): val(0) {};

		/// Constructs a number from raw data.
		/// @param data Number data.
		static fix raw(int64_t data) { fix f; f.val = data; return f; };
		/// Constructs a number from an integer.
		/// @param i Input integer.
		fix (int32_t i): val((int64_t)i << 32) {};
		/// Constructs a number from an floating point number.
		/// @param f Input float.
		fix (float f): val((int64_t)(f * 0x100000000)) {};

		/// Converts the number to an integer.
		/// @note Resulting integer will be rounded down.
		inline operator int32_t() const { return val >> 32; };
		/// Converts the number to a float.
		inline operator float() const { return (float)val / 0x100000000; };
		/// Returns byte representation of the integer.
		inline uint64_t bytes() const { return (uint64_t)val; };

		/// Returns a sum of 2 numbers.
		/// @param oth Other number.
		inline fix operator+(fix oth) const { return raw(val + oth.val); };
		/// Returns a difference of 2 numbers.
		/// @param oth Other number.
		inline fix operator-(fix oth) const { return raw(val - oth.val); };
		/// Returns a product of 2 numbers.
		/// @param oth Other number.
		inline fix operator*(fix oth) const { return raw(mul(val, oth.val)); };
		/// Returns a quotient of 2 numbers.
		/// @param oth Other number.
		inline fix operator/(fix oth) const { return raw(div(val, oth.val)); };
		/// Returns a modulo of 2 numbers.
		/// @param oth Other number.
		inline fix operator%(fix oth) const { return raw(mod(val, oth.val)); };
		/// Returns a bitwise AND of 2 numbers.
		/// @param oth Other number.
		inline fix operator&(fix oth) const { return raw(val & oth.val); };
		/// Returns a bitwise OR of 2 numbers.
		/// @param oth Other number.
		inline fix operator|(fix oth) const { return raw(val | oth.val); };
		/// Returns a bitwise XOR of 2 numbers.
		/// @param oth Other number.
		inline fix operator^(fix oth) const { return raw(val ^ oth.val); };

		/// Returns a left-shifted number.
		/// @param n Bits to shift.
		inline fix operator<<(uint8_t n) const { return raw(val << n); };
		/// Returns a right-shifted number.
		/// @param n Bits to shift.
		inline fix operator>>(uint8_t n) const { return raw(val >> n); };

		/// Adds a number.
		/// @param oth Other number.
		inline fix& operator+=(fix oth) { return *this = *this + oth; };
		/// Subtracts a number.
		/// @param oth Other number.
		inline fix& operator-=(fix oth) { return *this = *this - oth; };
		/// Multiplies by a number.
		/// @param oth Other number.
		inline fix& operator*=(fix oth) { return *this = *this * oth; };
		/// Divides by a number.
		/// @param oth Other number.
		inline fix& operator/=(fix oth) { return *this = *this / oth; };
		/// Limits a number.
		/// @param oth Other number.
		inline fix& operator%=(fix oth) { return *this = *this % oth; };
		/// ANDs a number.
		/// @param oth Other number.
		inline fix& operator&=(fix oth) { return *this = *this & oth; };
		/// ORs a number.
		/// @param oth Other number.
		inline fix& operator|=(fix oth) { return *this = *this | oth; };
		/// XORs a number.
		/// @param oth Other number.
		inline fix& operator^=(fix oth) { return *this = *this ^ oth; };

		/// Left-shifts the number.
		/// @param n Bits to shift.
		inline fix& operator<<=(uint8_t n) { return *this = *this << n; };
		/// Right-shifts the number.
		/// @param n Bits to shift.
		inline fix& operator>>=(uint8_t n) { return *this = *this >> n; };

		/// Returns the number.
		inline fix operator+() const { return *this; };
		/// Returns number's negation.
		inline fix operator-() const { return raw(-val); };
		/// Returns number's complementary.
		inline fix operator~() const { return raw(~val); };
		/// Returns a inverted boolean equivalent of the number.
		/// @note Equivalent to a `this == 0`.
		constexpr inline bool operator!() const { return !val; };
		/// Returns a boolean equivalent of the number.
		/// @note Equivalent to a `this != 0`.
		constexpr inline operator bool() const { return val != 0; };

		/// Returns number and increments it by 1.
		inline fix operator++(int) { fix now = *this; *this += 1; return now; };
		/// Returns number and decrements it by 1.
		inline fix operator--(int) { fix now = *this; *this -= 1; return now; };

		/// Increments number by 1 and returns the result.
		inline fix operator++() { return *this += 1; };
		/// Decrements number by 1 and returns the result.
		inline fix operator--() { return *this -= 1; };

		/// Checks if 2 numbers are equal.
		/// @param oth Other number.
		inline bool operator==(fix oth) const { return val == oth.val; };
		/// Checks if 2 numbers are not equal.
		/// @param oth Other number.
		inline bool operator!=(fix oth) const { return val != oth.val; };
		/// Checks if the number is less or equal than the other.
		/// @param oth Other number.
		inline bool operator<=(fix oth) const { return val <= oth.val; };
		/// Checks if the number is greater or equal than the other.
		/// @param oth Other number.
		inline bool operator>=(fix oth) const { return val >= oth.val; };
		/// Checks if the number is less than the other.
		/// @param oth Other number.
		inline bool operator<(fix oth) const { return val < oth.val; };
		/// Checks if the number is greater than the other.
		/// @param oth Other number.
		inline bool operator>(fix oth) const { return val > oth.val; };

		/// Returns a smaller value.
		/// @param oth Compared value.
		/// @return Smaller value.
		inline fix min(fix oth) const { return *this < oth ? *this : oth; };
		/// Returns a larger value.
		/// @param oth Compared value.
		/// @return Larger value.
		inline fix max(fix oth) const { return *this > oth ? *this : oth; };

		/// Clamps a value in a range.
		/// @param a Lower bound.
		/// @param b Upper bound.
		/// @return Clamped value.
		inline fix clamp(fix a, fix b) const { return max(a).min(b); };

		/// Returns number rounded down.
		inline fix floor() const { return raw(val & 0xFFFFFFFF00000000ULL); };
		/// Returns number rounded up.
		inline fix ceil() const { return (*this + raw(0xFFFFFFFFULL)).floor(); };
		/// Returns number rounded to nearest integer.
		inline fix round() const { return (*this + raw(0x80000000ULL)).floor(); };
		/// Returns number quotient value.
		inline fix quo() const { return raw(val & 0xFFFFFFFFULL); };
		/// Returns integer part of the number.
		inline fix whole() const { return val < 0 ? ceil() : floor(); };

		/// Returns an absolute value of the number.
		inline fix abs() const { return raw((val < 0) ? -val : val); };

		/// Checks if the number is in inclusive range.
		/// @param a Lower bound.
		/// @param b Upper bound.
		inline bool in(fix a, fix b) const { return a <= *this && *this <= b; };
		/// Performs a linear interpolation on an interval.
		/// @param a Start point.
		/// @param b End point.
		/// @return Interpolated value.
		inline fix lerp(fix a, fix b) const { return a + (b - a) * (*this); };

		/// Raises a number to an integer power.
		fix pow(int32_t x) const {
			if (x == 0) return fix(1);
			if (x < 0) return fix(1) / pow(-x);

			fix s = 0;
			fix a = *this;

			while (x > 0) {
				if (x & 1) { a *= a; x &= ~1; }
				else       { s *= a; x >>= 1; };
			};
			return s;
		};
};

/// Fixed point number literal generated from a double.
fix operator ""fx(long double val) { return fix::raw(val * 0x100000000); };
/// Fixed point number literal generated from a long integer.
fix operator ""fx(unsigned long long val) { return fix((int32_t)val); };