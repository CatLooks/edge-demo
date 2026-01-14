#pragma once

/// Level element structures namespace.
namespace Bytes {
	using Models::Theme;

	/// Block object.
	struct Block {
		/// Block data.
		uint8_t data;

		/// Block type enumeration.
		enum Type: uint8_t {
			None = 0,
			Full = 1,
			Half = 2
		};

		/// Default constructor.
		/// @param data Block data.
		/// @note Defaults to air.
		Block (uint8_t data = 0): data(data) {};

		/// Returns block type.
		Type type() const { return static_cast<Type>(data & 3); };
		/// Returns block theme.
		Theme theme() const { return static_cast<Theme>(data >> 2); };
		/// Retuns block volume.
		vol3x volume() const {
			switch (type()) {
				case Full: return vol3x(vec3x(0, 0, 0.0fx), vec3x(1, 1, 1.0fx)); break;
				case Half: return vol3x(vec3x(0, 0, 0.5fx), vec3x(1, 1, 0.5fx)); break;
				default: return vol3x();
			};
		};
	};

	/// Terrain object.
	struct Terrain {
		/// Terrain block data.
		Block* blocks = NULL;
		/// Terrain size.
		sf::Vector3i size;

		/// Default constructor.
		Terrain () {};
		/// Copy constructor.
		/// @param terrain Copied terrain object.
		Terrain (const Terrain& terrain): size(terrain.size) {
			blocks = memdup(terrain.blocks, sizeof(Block) * vol3(size));
		};
		/// Move constructor.
		/// @param terrain Moved terrain object.
		Terrain (Terrain&& terrain): blocks(terrain.blocks), size(terrain.size) {
			terrain.blocks = NULL;
		};
		/// Copy assignment.
		/// @param terrain Copied terrain object.
		Terrain& operator=(const Terrain& terrain) {
			size = terrain.size;
			blocks = memdup(terrain.blocks, sizeof(Block) * vol3(size));
			return *this;
		};
		/// Move assignment.
		/// @param terrain Moved terrain object.
		Terrain& operator=(Terrain&& terrain) {
			this->~Terrain();
			size = terrain.size;
			blocks = terrain.blocks;
			terrain.blocks = NULL;
			return *this;
		};
		/// Terrain destructor.
		~Terrain () { delete[] blocks; };

		/// Reads terrain data from a file.
		/// @param size Terrain size.
		/// @param file File reader.
		/// @return Terrain object.
		static Terrain read(sf::Vector3i size, FileReader& reader) {
			// allocate array
			Terrain terrain;
			terrain.blocks = new Block[vol3(size)];
			terrain.size = size;

			// create terrain bit reader
			BitReader bits(&reader);

			// read each block
			size_t i = 0;
			for (int z = 0; z < size.z; z++) {
				for (int y = 0; y < size.y; y++) {
					for (int x = 0; x < size.x; x++) {
						// read control bit
						bool exists = bits.read(1);
						if (exists) {
							// read block data
							bool half = bits.read(1);
							uint8_t theme = bits.read(2);

							// write block data
							terrain.blocks[i++] = Block(theme << 2 | (half ? 2 : 1));
						} else {
							// air block
							terrain.blocks[i++] = Block(0);
						};
					};
				};
			};
			return terrain;
		};

		/// Returns the block at specified position.
		/// @param pos Block position.
		/// @return Block at position.
		/// @note If position is out of bounds, air will be returned.
		Block at(sf::Vector3i pos) const {
			if (pos.x < 0 || pos.x >= size.x ||
				pos.y < 0 || pos.y >= size.y ||
				pos.z < 0 || pos.z >= size.z)
				return Block();
			return blocks[(pos.z * size.y + pos.y) * size.x + pos.x];
		};
	};
};