#pragma once

/// Level element structures namespace.
namespace Bytes {
	/// Cube event object.
	struct Event {
		/// Event direction.
		Direction dir;
		/// Event key type.
		bool key;
		/// Pause before the event.
		uint16_t time;
		/// Whether the event is a sync event.
		bool sync;
		/// Syncing moving platform index.
		uint16_t moving;
		/// Syncing moving platform waypoint index.
		uint16_t waypoint;

		/// Read event data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Event& read(FileReader& reader) {
			uint8_t data = reader.u8();
			dir = static_cast<Direction>(data & 3);
			key = data >> 2 & 1;
			sync = data >> 3 & 1;
			if (sync) {
				moving = reader.u16();
				waypoint = reader.u16();
			} else time = reader.u16();

			// log data
			Log::out.format("Offset = %d ticks, Event = %s %s", sync ? 0 : time, directionName(dir), key ? "Down" : "Up");
			if (sync) Log::out.format(" (Synced)");
			Log::out.queue("\n");
			Log::out.print();
			return *this;
		};
	};

	/// Cube input path object.
	struct Path {
		/// Path events.
		std::vector<Event> evts;

		/// Read path data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Path& read(FileReader& reader) {
			// read event count
			uint16_t count = reader.u16();
			Log::out.symI(); Log::out.format("Event count = %u\n", count);
			Log::out.print();

			// read events
			for (uint16_t i = 0; i < count; i++) {
				Log::out.symI(); Log::out.style(8);
				Log::out.format("Event No. %u: ", i + 1);
				Log::out.reset(); Log::out.print();
				evts.push_back(Event().read(reader));
			};
			return *this;
		};
	};
};