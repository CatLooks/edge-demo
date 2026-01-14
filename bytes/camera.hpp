#pragma once

/// Level element structures namespace.
namespace Bytes {
	/// Camera trigger object.
	struct CameraTrigger {
		vec3x     pos; /// Trigger position.
		vec2x     rad; /// Trigger radius.
		uint64_t time; /// Transition time.
		uint8_t  zoom; /// Trigger target zoom mode.
		uint8_t   fov; /// Trigger target FOV.

		/// Read camera trigger data.
		/// @param reader File reader object.
		/// @return Self-reference.
		CameraTrigger& read(FileReader& reader) {
			pos = readVec3(reader);
			rad = readVec2(reader);
			time = reader.u16();

			zoom = reader.u8();
			if (zoom == 0)
				fov = reader.u8();

			// log data
			Log::out.symI(); Log::out.format("Position = %d, %d, %d\n", (int)pos.x, (int)pos.y, (int)pos.z);
			Log::out.symI(); Log::out.format("Radius   = %d, %d\n", (int)rad.x, (int)rad.y);
			Log::out.symI(); Log::out.format("Time     = %u ticks\n", time);
			Log::out.symI(); Log::out.format("Mode     = %s\n", zoom == 0 ? "FOV" : "Mode");
			Log::out.symI();
			if (zoom) Log::out.format("Zoom     = %d\n", (int)zoom);
			else      Log::out.format("FOV      = %d*\n", (int)fov);

			return *this;
		};
	};

	/// Camera lock object.
	struct CameraLock {
		vec3x eye; /// Lock camera position.

		/// Read camera lock data.
		/// @param reader File reader object.
		/// @return Self-reference.
		CameraLock& read(FileReader& reader) {
			eye = readVec3(reader);

			// log data
			Log::out.symI(); Log::out.format("Position = %d, %d, %d\n", (int)eye.x, (int)eye.y, (int)eye.z);
			return *this;
		};
	};
};