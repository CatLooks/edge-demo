#pragma once

/// Level element structures namespace.
namespace Bytes {
	/// Moving platform command object.
	struct MovingCommand {
		uint16_t id;  // Moving platform index.
		uint16_t way; // Moving platform waypoint index.
	};

	/// Button command object.
	struct ButtonCommand {
		uint16_t id;  // Button index.
		bool  state; // Button state.
	};

	/// Bumper command object.
	struct BumperCommand {
		uint16_t            id; // Bumper index.
		Bumper::Activation act; // Bumper activation type.
	};

	/// Button object.
	struct Button {
		vec3x     pos; // Button position.
		vec2x     rad; // Button radius.
		bool    state; // Initial button state.
		bool  visible; // Button visibility.
		bool   camera; // Button camera effect.
		bool attached; // Whether the button is attached to a moving platform.
		bool   stayup; // Whether the button stays up.
		size_t moving; // Attached moving platform index.
		uint16_t lock; // Camera lock index.

		std::vector<MovingCommand> movings; // Moving platform commands.
		std::vector<ButtonCommand> buttons; // Button commands.
		std::vector<BumperCommand> bumpers; // Bumper commands.

		/// Read button data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Button& read(FileReader& reader) {
			// read button settings
			uint8_t data = reader.u8();
			state = data & 1;
			visible = data & 2;
			camera = data & 4;
			attached = data & 8;
			stayup = data & 16;

			// read button position
			if (attached) moving = reader.u16();
			else          pos = readVec3(reader);

			// read button radius
			if (!visible) rad = readVec2(reader);

			// read camera lock index
			if (camera) lock = reader.u16();

			// log data
			Log::out.symI();
			if (!attached)
				Log::out.format("Position = %d, %d, %d\n", (int)pos.x, (int)pos.y, (int)pos.z);
			else
				Log::out.format("Moving Platform Index = %u\n", moving);
			Log::out.symI(); Log::out.format("State    = %s\n", state ? "Pressed" : "Released");
			Log::out.symI(); Log::out.format("Mode     = %s\n", stayup ? "Stay Up" : "Stay Down");
			Log::out.symI(); Log::out.format("Draw     = %s\n", visible ? "Visible" : "Invisible");
			if (!visible) {
				Log::out.symI(); Log::out.format("Radius   = %d, %d\n", (int)rad.x, (int)rad.y);
			};
			if (camera) {
				Log::out.symI(); Log::out.format("Camera   = ");
				if (lock == 0xffff) Log::out.queue("Unlocked\n");
				else Log::out.format("Lock No. %u\n", lock);
			};
			Log::out.print();

			uint32_t count = reader.u16();
			Log::out.symI(); Log::out.format("Moving platform command count = %u\n", count);
			for (uint32_t i = 0; i < count; i++) {
				MovingCommand cmd = { reader.u16(), reader.u16() };
				movings.push_back(cmd);
				Log::out.symI(); Log::out.style(8); Log::out.format("No. %llu:\n", i + 1); Log::out.reset();
				Log::out.symI(); Log::out.format("ID = %d, Waypoint = %d\n", cmd.id, cmd.way);
				Log::out.print();
			};

			count = reader.u16();
			Log::out.symI(); Log::out.format("Button command count = %u\n", count);
			for (uint32_t i = 0; i < count; i++) {
				ButtonCommand cmd = { reader.u16(), (bool)reader.u8() };
				buttons.push_back(cmd);
				Log::out.symI(); Log::out.style(8); Log::out.format("No. %llu:\n", i + 1); Log::out.reset();
				Log::out.symI(); Log::out.format("ID = %d, State = %s\n", cmd.id, cmd.state ? "Pressed" : "Released");
				Log::out.print();
			};

			count = reader.u16();
			Log::out.symI(); Log::out.format("Bumper command count = %u\n", count);
			for (uint32_t i = 0; i < count; i++) {
				BumperCommand cmd = { reader.u16(), static_cast<Bumper::Activation>(reader.u8()) };
				bumpers.push_back(cmd);
				Log::out.symI(); Log::out.style(8); Log::out.format("No. %llu:\n", i + 1); Log::out.reset();
				Log::out.symI(); Log::out.format("ID = %d, Action = ", cmd.id);
				switch (cmd.act) {
					case Bumper::Disable: Log::out.queue("Disable\n");   break;
					case Bumper::Enable : Log::out.queue("Enable\n");    break;
					case Bumper::Once   : Log::out.queue("Tick Once\n"); break;
					default             : Log::out.queue("Unknown\n");   break;
				};
				Log::out.print();
			};
			return *this;
		};
	};
};