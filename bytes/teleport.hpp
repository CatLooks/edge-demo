#pragma once

/// Level element structures namespace.
namespace Bytes {
	/// Teleporter object.
	struct Teleport {
		vec3x pos; /// Teleporter position.
		vec2x rad; /// Teleporter radius.
		vec3x  tp; /// Teleporter destination.

		/// Read teleport data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Teleport& read(FileReader& reader) {
			pos = readVec3(reader);
			rad = readVec2(reader);
			tp = readVec3(reader);

			// log data
			Log::out.symI(); Log::out.format("Position = %d, %d, %d\n", (int)pos.x, (int)pos.y, (int)pos.z);
			Log::out.symI(); Log::out.format("Radius   = %d, %d\n", (int)rad.x, (int)rad.y);
			Log::out.symI(); Log::out.format("Teleport = %d, %d, %d\n", (int)tp.x, (int)tp.y, (int)tp.z);
			return *this;
		};
	};
};