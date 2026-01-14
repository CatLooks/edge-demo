#pragma once

/// Level related namespace.
namespace Level {
	using Models::Theme;
	using Bytes::Block;

	/// Terrain object.
	class Terrain {
		private:
		/// Terrain model.
		Object* object = NULL;

		/// Checks if a side should be drawn.
		/// @param Block with tested face.
		/// @param Neighbor block.
		static bool sideDraw(Block target, Block neighbor) {
			if (target.type() == Block::Full)
				return neighbor.type() != Block::Full;
			if (target.type() == Block::Half)
				return neighbor.type() == Block::None;
			return false;
		};

		/// Checks if a top surface should be drawn.
		/// @param Block with tested face.
		/// @param Neighbor block.
		static bool topDraw(Block target, Block neighbor) {
			return neighbor.type() != Block::Full;
		};

		/// Checks if a bottom surface should be drawn.
		/// @param Block with tested face.
		/// @param Neighbor block.
		static bool bottomDraw(Block target, Block neighbor) {
			if (target.type() == Block::Half)
				return true;
			return neighbor.type() == Block::None;
		};

		public:
		/// Terrain data.
		Bytes::Terrain root;

		/// Structure constructor.
		/// @param data Terrain data.
		Terrain (Bytes::Terrain data = {}): root(data) {};
		/// Copy constructor.
		/// @param terrain Copied terrain object.
		Terrain (const Terrain& terrain) = delete;
		/// Move constructor.
		/// @param terrain Moved terrain object.
		Terrain (Terrain&& terrain): object(terrain.object), root(terrain.root) {
			terrain.object = NULL;
		};
		/// Copy assignment.
		/// @param terrain Copied terrain object.
		Terrain& operator=(const Terrain& terrain) = delete;
		/// Move assignment.
		/// @param terrain Moved terrain object.
		Terrain& operator=(Terrain&& terrain) {
			this->~Terrain();
			object = terrain.object; terrain.object = NULL;
			root = terrain.root; terrain.root = {};
			return *this;
		};
		/// Terrain destructor.
		~Terrain () { delete object; };

		/// Renders terrain.
		/// @return Drawn vertex count.
		size_t draw() {
			if (object) {
				Assets::terrain.bind();
				return object->draw(Shaders::tex);
			};
			return 0;
		};

		/// Generates terrain model.
		/// @param level Level data.
		void model(const Bytes::Level* level) {
			// generate model buffer
			object = new Object;

			// fetch exit position
			sf::Vector3i exit = level->exit.int3();

			// generate vertices
			std::vector<Vertex> verts;
			for (int z = 0; z < root.size.z; z++) {
				for (int y = 0; y < root.size.y; y++) {
					for (int x = 0; x < root.size.x; x++) {
						// fetch block
						Block block = root.at({x, y, z});

						// ignore air
						if (block.type() == Block::None) continue;

						// generate rendering settings
						Models::Terrain::Settings settings;
						settings.half = block.type() == Block::Half;
						settings.pattern = (x ^ y ^ z) & 1;
						settings.shade = Textures::Terrain::shadeZ(z);
						settings.theme = block.theme();

						// side faces check
						settings.sides.xn = sideDraw(block, root.at({x - 1, y, z}));
						settings.sides.xp = sideDraw(block, root.at({x + 1, y, z}));
						settings.sides.zn = sideDraw(block, root.at({x, y - 1, z}));
						settings.sides.zp = sideDraw(block, root.at({x, y + 1, z}));

						// top & bottom face check
						bool exp = udiff(x, exit.x) <= 1 && udiff(y, exit.y) <= 1;
						settings.sides.yp = topDraw(block, root.at({x, y, z + 1})) && !(z == exit.z - 1 && exp);
						settings.sides.yn = bottomDraw(block, root.at({x, y, z - 1})) && !(z == exit.z && exp);

						// render block
						Models::Terrain::generate(verts, glm::vec3(x, z, y), settings);
					};
				};
			};

			// update model geometry
			object->set(verts);
			object->indexQuads();
			object->update();
		};
	};
};