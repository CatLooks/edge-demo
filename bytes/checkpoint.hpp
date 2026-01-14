#pragma once

/// Level element structures namespace.
namespace Bytes {
	using Models::Theme;

	/// Checkpoint object.
	struct Checkpoint {
		vec3x     pos; // Checkpoint position.
		vec2x     rad; // Checkpoint radius.
		vec3x     rsp; // Respawn position.
		uint16_t time; // Animation time.
		Theme   theme; // Gradient animation.
		bool    reset; // Whether to reset camera position.

		/// Read checkpoint data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Checkpoint& read(FileReader& reader) {
			pos = readVec3(reader);
			rad = readVec2(reader);
			rsp = readVec3(reader);

			reset = reader.u8();
			time = reader.u16();
			if (time != 0)
				theme = static_cast<Theme>(reader.u8());

			// log data
			Log::out.symI(); Log::out.format("Position  = %d, %d, %d\n", (int)pos.x, (int)pos.y, (int)pos.z);
			Log::out.symI(); Log::out.format("Radius    = %d, %d\n", (int)rad.x, (int)rad.y);
			Log::out.symI(); Log::out.format("Respawn   = %d, %d, %d\n", (int)rsp.x, (int)rsp.y, (int)rsp.z);
			if (time != 0) {
				Log::out.symI(); Log::out.format("Theme     = %s\n", Models::themeRepr(theme));
				Log::out.symI(); Log::out.format("Time      = %u ticks\n", time);
			};
			Log::out.print();
			return *this;
		};
	};
};