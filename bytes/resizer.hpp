#pragma once

/// Level element structures namespace.
namespace Bytes {
	/// Resizer object.
	struct Resizer {
		/// Resizer mode type.
		enum Mode {
			Shrink,
			Grow
		};

		vec3x    pos; // Resizer position.
		vec2x    rad; // Resizer radius of effect.
		Mode    mode; // Resizer mode.
		bool visible; // Resizer visibility.

		/// Read resizer data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Resizer& read(FileReader& reader) {
			pos = readVec3(reader);
			rad = readVec2(reader);

			uint8_t data = reader.u8();
			mode = static_cast<Mode>(data & 1);
			visible = data & 2;

			// log data
			Log::out.symI(); Log::out.format("Mode     = %s\n", mode == Shrink ? "Shrinker" : "Grower");
			Log::out.symI(); Log::out.format("Position = %d, %d, %d\n", (int)pos.x, (int)pos.y, (int)pos.z);
			Log::out.symI(); Log::out.format("Radius   = %d, %d\n", (int)rad.x, (int)rad.y);
			Log::out.symI(); Log::out.format("Visible  = %c\n", visible ? 'T' : 'F');
			Log::out.print();
			return *this;
		};

		/// Checks whether the resizer is a shrinker.
		bool shrinks() const { return mode == Shrink; };
	};
};