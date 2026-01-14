#pragma once

/// Model generation namespace.
namespace Models {
	/// Terrain modeling.
	namespace Terrain {
		// import terrain texture types
		using Textures::Terrain::Sides;
		using Textures::Terrain::TexLevel;

		/// Terrain block generation settings.
		struct Settings {
			Sides   sides; /// Drawn sides.
			Theme   theme; /// Block theme.
			uint8_t shade; /// Block shading.
			bool  pattern; /// Block surface pattern.
			bool     half; /// Block height.
		};

		/// Generates a terrain block model.
		/// @param verts Output vertices.
		/// @param offset Model offset.
		/// @param settings Model settings.
		void generate(std::vector<Vertex>& verts, glm::vec3 offset, const Settings& settings) {
			Vertex buffer[4];

			// calculate rectangle limit vectors
			glm::vec3 min = offset + glm::vec3(-0.5f, settings.half ? 0.0f : -0.5f, -0.5f);
			glm::vec3 max = offset + glm::vec3(0.5f);

			// get block textures
			const TexLevel* level = &Textures::Terrain::texMaps[settings.theme][settings.shade];

			// "X-" face
			if (settings.sides.xn) {
				for (GLuint i = 0; i < 4; i++)
					buffer[i] = Vertex(vertID(min, max, faces[0][i]), level->columnX[settings.half].coords[i]);
				verts.insert(verts.end(), buffer, buffer + 4);
			};
			// "X+" face
			if (settings.sides.xp) {
				for (GLuint i = 0; i < 4; i++)
					buffer[i] = Vertex(vertID(min, max, faces[1][i]), level->columnX[settings.half].coords[i]);
				verts.insert(verts.end(), buffer, buffer + 4);
			};
			// "Z-" face
			if (settings.sides.zn) {
				for (GLuint i = 0; i < 4; i++)
					buffer[i] = Vertex(vertID(min, max, faces[2][i]), level->columnZ[settings.half].coords[i]);
				verts.insert(verts.end(), buffer, buffer + 4);
			};
			// "Z+" face
			if (settings.sides.zp) {
				for (GLuint i = 0; i < 4; i++)
					buffer[i] = Vertex(vertID(min, max, faces[3][i]), level->columnZ[settings.half].coords[i]);
				verts.insert(verts.end(), buffer, buffer + 4);
			};
			// "Y-" face
			if (settings.sides.yn) {
				for (GLuint i = 0; i < 4; i++)
					buffer[i] = Vertex(vertID(min, max, faces[4][i]), level->floor[settings.half].coords[i]);
				verts.insert(verts.end(), buffer, buffer + 4);
			};
			// "Y+" face
			if (settings.sides.yp) {
				for (GLuint i = 0; i < 4; i++)
					buffer[i] = Vertex(vertID(min, max, faces[5][i]), level->surface[settings.pattern].coords[i]);
				verts.insert(verts.end(), buffer, buffer + 4);
			};
		};
	};
};