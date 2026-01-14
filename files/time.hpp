#pragma once

/// Data storage namespace.
namespace Data {
	/// Level ranking type.
	enum Rank {
		S = 0, A, B, C, D, N
	};

	/// Returns raw rank string representation.
	/// @param rank Ranking value.
	const char* rawRankRepr(Rank rank) {
		switch (rank) {
			case S : return "S";
			case A : return "A";
			case B : return "B";
			case C : return "C";
			case D : return "D";
			default: return "-";
		};
	};

	/// Returns rank string representation.
	/// @param rank Ranking value.
	const char* rankRepr(Rank rank) {
		switch (rank) {
			case S : return "\6S\7";
			case A : return "A";
			case B : return "B";
			case C : return "C";
			case D : return "\x14" "D\r";
			default: return "\x14-\r";
		};
	};

	/// Returns average rank string representation.
	/// @param acc Inverted ranking accumulator.
	/// @param count Rank count.
	const char* avgRankRepr(uint32_t acc, uint32_t count) {
		// ranking strings
		const char* table[12] {
			"\x14" "D\r", "\x14" "D+\r", "C-",
			"C", "C+", "B-",
			"B", "B+", "\6A-\7",
			"\6A\7", "\6A+\7", "\6A*\7"
		};
		const char* perf = "\x17\6S\7";
		const char* none = "\x14-\r";

		// check for no rank
		if (count == 0) return none;

		// check for perfect rank
		if (acc == count * 4) return perf;

		// compute normalized accumulator values
		float t = (float)acc / count;

		// check all rank subdivisions
		for (uint32_t i = 0; i < 11; i++) {
			if (t <= i / 3.0f)
				return table[i];
		};
		return table[11];
	};

	/// Level timer.
	struct Time {
		uint32_t game = 0; /// Gametime.
		uint32_t edge = 0; /// Edgetime.

		/// Updates game timer.
		/// @param delta Time elapsed since last frame.
		void updateGame(float delta) { game += delta * 1000.0f; };
		/// Updates edge timer.
		/// @param delta Time elapsed since last frame.
		void updateEdge(float delta) { edge += delta * 1000.0f; };

		/// Returns the millisecond part of time.
		static unsigned int ms(uint32_t time) { return time % 1000; };
		/// Returns the second part of time.
		static unsigned int sec(uint32_t time) { return (time / 1000) % 60; };
		/// Returns the minute part of time.
		static unsigned int min(uint32_t time) { return time / 60000; };

		/// Returns total level time.
		uint32_t total() const { return game - edge; };

		/// Returns string representation of time.
		static char* repr(uint32_t time) {
			if (min(time) == 0)
				return formatf("%d'%03d", sec(time), ms(time));
			return formatf("%d>%02d'%03d", min(time), sec(time), ms(time));
		};
	};

	/// Time thresholds structure.
	struct Thresholds {
		uint16_t time[D] {0};

		/// Read threshold data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Thresholds& read(FileReader& reader) {
			for (size_t i = 0; i < D; i++)
				time[i] = reader.u16();
			return *this;
		};

		/// Rate the game based on level time.
		/// @param full Whether to use full ranking scale.
		Rank rate(uint32_t timer, bool full) const {
			if (full)
				if (timer < time[S] * 1000) return S;
			if (timer < time[A] * 1000) return A;
			if (timer < time[B] * 1000) return B;
			if (timer < time[C] * 1000) return C;
			return D;
		};
	};
};