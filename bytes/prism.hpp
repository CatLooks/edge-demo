#pragma once

/// Level element structures namespace.
namespace Bytes {
	/// Prism object.
	struct Prism {
		vec3x pos; // Prism position.

		/// Read prism data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Prism& read(FileReader& reader) {
			pos = readVec3(reader);

			// log data
			Log::out.symI(); Log::out.format("Position = %d, %d, %d\n", (int)pos.x, (int)pos.y, (int)pos.z);
			Log::out.print();
			return *this;
		};
	};
};