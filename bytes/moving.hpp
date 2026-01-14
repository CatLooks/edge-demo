#pragma once

/// Level element structures namespace.
namespace Bytes {
	using Models::Theme;

	/// Waypoint object.
	struct Waypoint {
		/// Action after waypoint type.
		enum After {
			Halt,
			Next,
			Loop
		};

		vec3x     dest; // Waypoint destination.
		uint64_t  time; // Waypoint duration.
		uint64_t total; // Total waypoint time.
		After    after; // Action after waypoint.

		/// Read waypoint data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Waypoint& read(FileReader& reader) {
			dest  = readVec3(reader);
			time = reader.u16();
			total = time + reader.u16();
			after = static_cast<After>(reader.u8());

			// log data
			Log::out.symI(); Log::out.format("Destination  = %d, %d, %d\n", (int)dest.x, (int)dest.y, (int)dest.z);
			Log::out.symI(); Log::out.format("Travel Time  = %u ticks\n", time);
			Log::out.symI(); Log::out.format("Pause Time   = %u ticks\n", total - time);
			Log::out.symI(); Log::out.format("After Action = ");
			switch (after) {
				case Waypoint::Halt: Log::out.queue("Halt\n"); break;
				case Waypoint::Next: Log::out.queue("Next\n"); break;
				case Waypoint::Loop: Log::out.queue("Loop\n"); break;
				default: Log::out.queue("?\n"); break;
			};
			Log::out.print();
			return *this;
		};
	};

	/// Moving platform object.
	struct Moving {
		vec3x                start; // Platform starting position.
		std::vector<Waypoint> ways; // Waypoint list.
		Theme                theme; // Platform model theme.
		bool                  half; // Platform height.
		bool                active; // Platform initial mode.

		/// Read moving platform data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Moving& read(FileReader& reader) {
			start = readVec3(reader);

			uint8_t data = reader.u8();
			half = data >> 3;
			theme = static_cast<Theme>(data & 3);
			active = !(data >> 2 & 1);

			// log data
			Log::out.symI(); Log::out.format("Start = %d, %d, %d\n", (int)start.x, (int)start.y, (int)start.z);
			Log::out.symI(); Log::out.format("Theme = %s\n", Models::themeRepr(theme));
			Log::out.symI(); Log::out.format("State = %s\n", active ? "Active" : "Inactive");
			Log::out.symI(); Log::out.format("Size  = %s\n", half ? "Half" : "Full");
			Log::out.print();

			size_t count = reader.u16();
			Log::out.symI(); Log::out.format("Waypoint count = %u\n", count);
			for (size_t i = 0; i < count; i++) {
				Log::out.symI(); Log::out.style(8); Log::out.format("No. %llu:\n", i + 1); Log::out.reset();
				ways.push_back(Waypoint().read(reader));
			};
			return *this;
		};
	};
};