#pragma once

/// Level element structures namespace.
namespace Bytes {
	using Models::Theme;

	/// Level route object.
	struct Route {
		vec3x  pos; /// Route position.
		bool  mini; /// Route cube state.
		Theme grad; /// Background gradient.
		Path  path; /// Path to next route.

		/// Button press request list.
		std::vector<size_t> buttons;

		/// Route level file name.
		std::string file;

		/// Read route data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Route& read(FileReader& reader) {
			pos = readVec3(reader);
			uint8_t data = reader.u8();
			mini = data >> 2;
			grad = static_cast<Theme>(data & 3);
			path = Path().read(reader);

			// read button effects
			uint32_t count = reader.u16();
			Log::out.symI(); Log::out.format("Button effect count = %u\n", count);
			for (uint32_t i = 0; i < count; i++) {
				uint16_t button = reader.u16();
				buttons.push_back(button);
				Log::out.symI(); Log::out.style(8); Log::out.format("No. %llu: ", i + 1); Log::out.reset();
				Log::out.symI(); Log::out.format("ID = %u\n", button);
				Log::out.print();
			};

			// read route file
			file = std::string();
			while (char c = reader.u8())
				file += c;

			// log data
			Log::out.symI(); Log::out.format("Position = %d, %d, %d\n", (int)pos.x, (int)pos.y, (int)pos.z);
			Log::out.symI(); Log::out.format("State    = %s\n", mini ? "Mini" : "Normal");
			Log::out.symI(); Log::out.format("Target   = <%s>\n", file.c_str());
			return *this;
		};
	};

	/// Route collection object.
	struct Routes {
		/// World map routes.
		std::vector<Route> routes;

		/// Dark prism repaint indices.
		std::vector<uint16_t> dark_prisms;

		/// Read route collection data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Routes& read(FileReader& reader) {
			Log::out.separator("Route Data");
			Log::out.print();

			// read route count
			uint32_t count = reader.u16();
			Log::out.symI(); Log::out.format("Route count = %u\n", count);
			for (uint32_t i = 0; i < count; i++) {
				Log::out.symI(); Log::out.style(8); Log::out.format("No. %llu:\n", i + 1); Log::out.reset();
				routes.push_back(Route().read(reader));
				Log::out.print();
			};

			// read dark prism count
			count = reader.u16();
			Log::out.symI(); Log::out.format("Dark prism count = %u\n", count);
			for (uint32_t i = 0; i < count; i++) {
				Log::out.symI(); Log::out.style(8); Log::out.format("No. %llu: ", i + 1); Log::out.reset();
				uint16_t idx = reader.u16();
				dark_prisms.push_back(idx);
				Log::out.format("Index = %u\n", idx);
				Log::out.print();
			};
			return *this;
		};
	};
};