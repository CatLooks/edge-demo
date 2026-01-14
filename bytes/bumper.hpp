#pragma once

/// Level element structures namespace.
namespace Bytes {
	using Models::Theme;

	/// Bumper object.
	struct Bumper {
		/// Bumper activation type.
		enum Activation {
			Disable,
			Enable,
			Once
		};

		vec3x     pos; // Bumper position.
		Direction dir; // Bumper direction.
		Theme   theme; // Bumper model theme.
		bool   active; // Bumper initial activity.
		uint16_t time; // Bumper bump interval.
		uint16_t bump; // Bumper bump time.
		uint16_t init; // Pause before first bump.
		uint8_t power; // Bumper bump power.

		/// Read bumper data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Bumper& read(FileReader& reader) {
			pos = readVec3(reader);

			uint8_t data = reader.u8();
			dir = static_cast<Direction>(data & 3);
			theme = static_cast<Theme>(data >> 2 & 3);
			active = data >> 4;

			time = reader.u16();
			bump = reader.u16();
			init = reader.u16();
			power = reader.u8();

			// log data
			Log::out.symI(); Log::out.format("Position  = %d, %d, %d\n", (int)pos.x, (int)pos.y, (int)pos.z);
			Log::out.symI(); Log::out.format("Direction = %s\n", directionName(dir));
			Log::out.symI(); Log::out.format("Theme     = %s\n", Models::themeRepr(theme));
			Log::out.symI(); Log::out.format("State     = %s\n", active ? "Active" : "Inactive");
			Log::out.symI(); Log::out.format("Interval  = %u ticks\n", time);
			Log::out.symI(); Log::out.format("Bump Time = %u ticks\n", bump);
			Log::out.symI(); Log::out.format("Pause     = %u ticks\n", init);
			Log::out.symI(); Log::out.format("Impulse   = %u\n", power);
			return *this;
		};
	};
};