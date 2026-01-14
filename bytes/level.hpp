#pragma once

/// Level element structures namespace.
namespace Bytes {
	/// Reads a 2D vector value from file.
	/// @param reader File reader object.
	vec2x readVec2(FileReader& reader) {
		return vec2x {
			(fix)reader.u8(),
			(fix)reader.u8()
		};
	};

	/// Reads a 3D vector value from file.
	/// @param reader File reader object.
	vec3x readVec3(FileReader& reader) {
		return vec3x {
			(fix)reader.s16(),
			(fix)reader.s16(),
			(fix)reader.s16()
		};
	};
};

// import element units
#include "terrain.hpp"
#include "moving.hpp"
#include "falling.hpp"
#include "bumper.hpp"
#include "resizer.hpp"
#include "prism.hpp"
#include "checkpoint.hpp"
#include "button.hpp"
#include "camera.hpp"
#include "teleport.hpp"
#include "path.hpp"
#include "routes.hpp"
#include "darkcube.hpp"

/// Level element structures namespace.
namespace Bytes {
	/// Level signature object.
	struct Sign {
		/// Level ID.
		uint16_t ID;
		/// Level name.
		std::string name;
		/// Level time thresholds.
		Data::Thresholds thresholds;
		/// Level prism count.
		uint16_t prisms;

		/// Reads signature data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Sign& read(FileReader& reader) {
			// read ID
			ID = reader.u16();

			// read name
			name = std::string();
			while (char c = reader.u8())
				name += c;

			// read time thresholds
			thresholds.read(reader);

			// read prism count
			prisms = reader.u16();
			return *this;
		};
	};

	/// Level object.
	struct Level {
		uint16_t      ID; // Level ID.
		std::string name; // Level name.
		Terrain  terrain; // Level terrain data.
		vec3x      spawn; // Level spawn point.
		vec3x       exit; // Level exit point.
		uint8_t     zoom; // Initial level zoom.
		Theme         bg; // Initial background color.
		Music::Music mus; // Level music.

		/// Level thresholds.
		Data::Thresholds thresholds;
		/// Level prism count.
		uint16_t prismCount;

		std::vector<Moving>         movings; // Moving platform list.
		std::vector<Falling>       fallings; // Falling platform list.
		std::vector<Bumper>         bumpers; // Bumper list.
		std::vector<Resizer>       resizers; // Resizer list.
		std::vector<Prism>           prisms; // Prism list.
		std::vector<Checkpoint>      checks; // Checkpoint list.
		std::vector<Button>         buttons; // Button list.
		std::vector<CameraTrigger> triggers; // Camera trigger list.
		std::vector<CameraLock>       locks; // Camera lock list.
		std::vector<Teleport>         ports; // Teleporter list.
		std::vector<DarkCube>     darkcubes; // Dark cube spawner list.

		/// Reads level data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Level& read(FileReader& reader) {
			Log::out.separator("Level Data");
			Log::out.print();

			// read level signature
			Sign sign = Sign().read(reader);
			ID = sign.ID;
			name = sign.name;
			thresholds = sign.thresholds;
			prismCount = sign.prisms;

			// read level size
			sf::Vector3i size = readVec3(reader).int3();

			// read level spawn & exit
			spawn = readVec3(reader);
			exit  = readVec3(reader);

			// read level zoom
			zoom = reader.u8();

			// read background
			bg = static_cast<Models::Theme>(reader.u8());

			// read level music
			mus = static_cast<Music::Music>(reader.u8());

			// log data
			Log::out.symI(); Log::out.format("ID    = %u\n", ID);
			Log::out.symI(); Log::out.format("Name  = %s\n", name.c_str());
			Log::out.symI(); Log::out.format("Size  = %d, %d, %d\n", size.x, size.y, size.z);
			Log::out.symI(); Log::out.format("Spawn = %d, %d, %d\n", (int)spawn.x, (int)spawn.y, (int)spawn.z);
			Log::out.symI(); Log::out.format("Exit  = %d, %d, %d\n", (int)exit.x, (int)exit.y, (int)exit.z);
			Log::out.symI(); Log::out.format("Zoom  = %d\n", zoom);
			Log::out.symI(); Log::out.format("Space = %s\n", Models::themeRepr(bg));
			Log::out.symI(); Log::out.format("Music = %s\n", Music::getFile(mus));
			Log::out.print();

			// read terrain data
			terrain = Terrain::read(size, reader);

			// read moving platform data
			uint32_t m_count = reader.u16();
			Log::out.symI(); Log::out.format("Moving platform count = %u\n", m_count); Log::out.print();
			for (uint32_t i = 0; i < m_count; i++) {
				Log::out.symI(); Log::out.style(8);
				Log::out.format("Moving platform No. %u\n", i + 1);
				Log::out.reset(); Log::out.print();
				movings.push_back(Moving().read(reader));
			};

			// read falling platform data
			uint32_t f_count = reader.u16();
			Log::out.symI(); Log::out.format("Falling platform count = %u\n", f_count); Log::out.print();
			for (uint32_t i = 0; i < f_count; i++) {
				Log::out.symI(); Log::out.style(8);
				Log::out.format("Falling platform No. %u\n", i + 1);
				Log::out.reset(); Log::out.print();
				fallings.push_back(Falling().read(reader));
			};

			// read resizer data
			uint32_t b_count = reader.u16();
			Log::out.symI(); Log::out.format("Bumper count = %u\n", b_count); Log::out.print();
			for (uint32_t i = 0; i < b_count; i++) {
				Log::out.symI(); Log::out.style(8);
				Log::out.format("Bumper No. %u\n", i + 1);
				Log::out.reset(); Log::out.print();
				bumpers.push_back(Bumper().read(reader));
			};

			// read resizer data
			uint32_t r_count = reader.u16();
			Log::out.symI(); Log::out.format("Resizer count = %u\n", r_count); Log::out.print();
			for (uint32_t i = 0; i < r_count; i++) {
				Log::out.symI(); Log::out.style(8);
				Log::out.format("Resizer No. %u\n", i + 1);
				Log::out.reset(); Log::out.print();
				resizers.push_back(Resizer().read(reader));
			};

			// read prism data
			Log::out.symI(); Log::out.format("Prism count = %u\n", prismCount); Log::out.print();
			for (uint32_t i = 0; i < prismCount; i++) {
				Log::out.symI(); Log::out.style(8);
				Log::out.format("Prism No. %u\n", i + 1);
				Log::out.reset(); Log::out.print();
				prisms.push_back(Prism().read(reader));
			};

			// read checkpoint data
			uint32_t c_count = reader.u16();
			Log::out.symI(); Log::out.format("Checkpoint count = %u\n", c_count); Log::out.print();
			for (uint32_t i = 0; i < c_count; i++) {
				Log::out.symI(); Log::out.style(8);
				Log::out.format("Checkpoint No. %u\n", i + 1);
				Log::out.reset(); Log::out.print();
				checks.push_back(Checkpoint().read(reader));
			};

			// read button data
			uint32_t a_count = reader.u16();
			Log::out.symI(); Log::out.format("Button count = %u\n", a_count); Log::out.print();
			for (uint32_t i = 0; i < a_count; i++) {
				Log::out.symI(); Log::out.style(8);
				Log::out.format("Button No. %u\n", i + 1);
				Log::out.reset(); Log::out.print();
				buttons.push_back(Button().read(reader));
			};

			// read camera trigger data
			uint32_t t_count = reader.u16();
			Log::out.symI(); Log::out.format("Camera trigger count = %u\n", t_count); Log::out.print();
			for (uint32_t i = 0; i < t_count; i++) {
				Log::out.symI(); Log::out.style(8);
				Log::out.format("Camera trigger No. %u\n", i + 1);
				Log::out.reset(); Log::out.print();
				triggers.push_back(CameraTrigger().read(reader));
			};

			// read camera lock data
			uint32_t l_count = reader.u16();
			Log::out.symI(); Log::out.format("Camera lock count = %u\n", l_count); Log::out.print();
			for (uint32_t i = 0; i < l_count; i++) {
				Log::out.symI(); Log::out.style(8);
				Log::out.format("Camera lock No. %u\n", i + 1);
				Log::out.reset(); Log::out.print();
				locks.push_back(CameraLock().read(reader));
			};

			// read camera lock data
			uint32_t q_count = reader.u16();
			Log::out.symI(); Log::out.format("Teleporter count = %u\n", q_count); Log::out.print();
			for (uint32_t i = 0; i < q_count; i++) {
				Log::out.symI(); Log::out.style(8);
				Log::out.format("Teleporter No. %u\n", i + 1);
				Log::out.reset(); Log::out.print();
				ports.push_back(Teleport().read(reader));
			};

			// read camera lock data
			uint32_t d_count = reader.u16();
			Log::out.symI(); Log::out.format("Dark cube count = %u\n", d_count); Log::out.print();
			for (uint32_t i = 0; i < d_count; i++) {
				Log::out.symI(); Log::out.style(8);
				Log::out.format("Dark cube No. %u\n", i + 1);
				Log::out.reset(); Log::out.print();
				darkcubes.push_back(DarkCube().read(reader));
			};

			// log end
			Log::out.symI();
			Log::out.queue("Level loading finished.\n");
			Log::out.print();
			return *this;
		};
	};
};