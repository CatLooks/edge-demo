#pragma once

/// Model generation namespace.
namespace Models {
	/// Moving platform models.
	namespace Moving {
		/// Theme shade level colors.
		struct Level {
			glm::vec3 up;     /// Light side color.
			glm::vec3 down;   /// Dark side color.
			glm::vec3 top;    /// Top surface color.
			glm::vec3 bot[2]; /// Bottom surface color (full & half).
		};

		/// Moving platform static colors.
		Level levels[4][4];

		/// Loads highlight colors from texture.
		void loadColors() {
			// coordinate multiplier
			const sf::Vector2u size = sf::Vector2u(512, 256);

			// load colors
			for (size_t t = 0; t < 4; t++) {
				for (size_t s = 0; s < 4; s++) {
					levels[t][s].up     = Assets::pixel(Assets::Mid::terrain, sf::Vector2u(t * 64 + 32 , s * 64      ), size) * 0.8f;
					levels[t][s].down   = Assets::pixel(Assets::Mid::terrain, sf::Vector2u(t * 64 + 32 , s * 64 + 63 ), size) * 0.8f;
					levels[t][s].top    = Assets::pixel(Assets::Mid::terrain, sf::Vector2u(t * 64 + 256, s * 32      ), size);
					levels[t][s].bot[0] = Assets::pixel(Assets::Mid::terrain, sf::Vector2u(t * 64 + 256, s * 32 + 128), size);
					levels[t][s].bot[1] = Assets::pixel(Assets::Mid::terrain, sf::Vector2u(t * 64 + 288, s * 32 + 128), size);
				};
			};
		};

		/// Generates a moving platform model.
		/// @param object Moving platform model.
		/// @param theme Platform theme.
		/// @param half Platform height.
		void platform(Object& object, Theme theme, bool half) {
			// create default platform rendering settings
			Terrain::Settings settings;
			settings.half = half;
			settings.pattern = 0;
			settings.shade = 0;
			settings.theme = theme;

			// create platform
			std::vector<Vertex> verts;
			Terrain::generate(verts, glm::vec3(0.0f), settings);

			// update Y-axis side colors
			for (GLuint f = 4; f < 6; f++)
				for (GLuint i = 0; i < 4; i++)
					verts[f * 4 + i].tex = Textures::Terrain::white.coords[i];

			// generate geometry
			object.set(verts);
			object.indexQuads();
		};

		/// Updates a moving platform model.
		/// @param object Moving platform model.
		/// @param theme Platform theme.
		/// @param half Platform height.
		/// @param y Vertical platform position.
		void update(Object& object, Theme theme, bool half, float y) {
			// get base shade level texture maps
			const Textures::Terrain::TexLevel* level = &Textures::Terrain::texMaps[theme][0];

			// calculate texture coordinates offset
			glm::vec2 off = { 0.0f, (3.0f - y) * 0.25f };

			// calculate surface color
			glm::vec3 col[2];
			if (y < 1.0f) {
				// dark platforms
				col[0] = levels[theme][3].bot[half];
				col[1] = levels[theme][3].top;
			} else if (y < 3.0f) {
				// calculate gradient
				float t = fmod(3.0f - y, 1.0f);
				GLuint s = ceilf(3.0f - y);

				// gradient platforms
				col[0] = lerpv(levels[theme][s - 1].bot[half], levels[theme][s].bot[half], t);
				col[1] = lerpv(levels[theme][s - 1].top, levels[theme][s].top, t);
			} else {
				// light platforms
				col[0] = levels[theme][0].bot[half];
				col[1] = levels[theme][0].top;
			};

			// update X-axis sides
			for (GLuint f = 0; f < 2; f++)
				for (GLuint i = 0; i < 4; i++)
					object.vertices()[f * 4 + i].tex = level->columnX[half].coords[i] + off;

			// update Z-axis sides
			for (GLuint f = 2; f < 4; f++)
				for (GLuint i = 0; i < 4; i++)
					object.vertices()[f * 4 + i].tex = level->columnZ[half].coords[i] + off;

			// update Y-axis sides
			for (GLuint f = 4; f < 6; f++)
				for (GLuint i = 0; i < 4; i++)
					object.vertices()[f * 4 + i].col = col[f & 1];

			// update geometry
			object.update();
		};

		/// Generates an active moving platform highlight model.
		/// @param object Moving platform highlight model.
		/// @param half Moving platform height.
		void highlight(Object& object, bool half) {
			Vertex buffer[24];

			// moving platform dimensions
			glm::vec3 min = glm::vec3(-0.5f, half ? 0.0f : -0.5f, -0.5f) - offsetXYZ / 2.0f;
			glm::vec3 max = glm::vec3(0.5f) + offsetXYZ / 2.0f;

			// generate vertices
			for (GLuint f = 0; f < 6; f++) {
				for (GLuint i = 0; i < 4; i++)
					buffer[f * 4 + i] = Vertex(vertID(min, max, faces[f][i]), ((half && f < 4) ? Textures::Mask::whiteH : Textures::Mask::whiteF).coords[i]);
			};

			// update object geometry
			object.set(buffer, lengthof(buffer));
			object.indexQuads();
			object.update();
		};
	};
};