#pragma once

/// Level related namespace.
namespace Bytes {
	/// Dark cube spawner object.
	struct DarkCube {
		vec3x pos; /// Spawner position.
		vec2x rad; /// Spawner radius.

		vec3x spawn; /// Dark cube starting position.
		bool   mini; /// Whether the dark cube should be small.
		bool   dark; /// Dark cube color.
		Path   path; /// Dark cube path.

		/// Read dark cube spawner data.
		/// @param reader File reader object.
		/// @return Self-reference.
		DarkCube& read(FileReader& reader) {
			pos = readVec3(reader);
			rad = readVec2(reader);

			spawn = readVec3(reader);
			uint8_t data = reader.u8();
			mini = data & 1;
			dark = !(data & 2);

			// log data
			Log::out.symI(); Log::out.format("Position = %d, %d, %d\n", (int)pos.x, (int)pos.y, (int)pos.z);
			Log::out.symI(); Log::out.format("Radius   = %d, %d\n", (int)rad.x, (int)rad.y);
			Log::out.symI(); Log::out.format("Spawn    = %d, %d, %d\n", (int)spawn.x, (int)spawn.y, (int)spawn.z);
			Log::out.symI(); Log::out.format("State    = %s\n", mini ? "Mini" : "Normal");
			Log::out.symI(); Log::out.format("Color    = %s\n", dark ? "Dark" : "Rainbow");

			path = Path().read(reader);
			return *this;
		};
	};
};