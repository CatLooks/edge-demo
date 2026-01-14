#pragma once

/// Level element structures namespace.
namespace Bytes {
	using Models::Theme;

	/// Falling platform object.
	struct Falling {
		vec3x     pos; // Platform position.
		uint64_t time; // Platform float time.
		Theme   theme; // Platform model theme.

		/// Read falling platform data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Falling& read(FileReader& reader) {
			pos = readVec3(reader);
			time = reader.u16();
			theme = static_cast<Theme>(reader.u8() & 3);

			// log data
			Log::out.symI(); Log::out.format("Position   = %d, %d, %d\n", (int)pos.x, (int)pos.y, (int)pos.z);
			Log::out.symI(); Log::out.format("Float Time = %u ticks\n", time);
			Log::out.symI(); Log::out.format("Theme      = %s\n", Models::themeRepr(theme));
			return *this;
		};
	};
};