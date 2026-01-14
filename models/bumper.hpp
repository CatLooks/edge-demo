#pragma once

/// Model generation namespace.
namespace Models {
	/// Bumper modeling.
	namespace Bumper {
		// import terrain texture types
		using Textures::Terrain::TexLevel;
		using Textures::TexMap;

		/// Bumper max bump size.
		const float size = 7 / 16.0f;

		/// Generates a bumper model.
		/// @param object Bumper model.
		/// @param dir Bumper direction.
		/// @param theme Bumper theme.
		/// @param shade Bumper shading.
		void generate(Object& object, Direction dir, Theme theme, uint8_t shade) {
			// get bumper textures
			const TexLevel* level = &Textures::Terrain::texMaps[theme][shade];
			const TexMap& side = (dir & 2) ? level->columnZ[0] : level->columnX[0];

			// bumper base cube dimensions
			glm::vec3 min = glm::vec3(-0.5f);
			glm::vec3 max = glm::vec3(0.5f);

			// generate vertex positions
			Vertex verts[4];
			for (GLuint i = 0; i < 4; i++)
				verts[i] = Vertex(vertID(min, max, faces[dir][i]), side.coords[i]);

			// generate middle vertex
			Vertex mid(midv(verts[0].pos, verts[3].pos), side.center());

			// generate vertices
			Vertex buffer[12] {
				verts[0], verts[1], mid,
				verts[1], verts[3], mid,
				verts[3], verts[2], mid,
				verts[2], verts[0], mid
			};

			// update geometry
			object.set(buffer, lengthof(buffer));
			object.indexTriangles();
		};

		/// Animates a bumper model.
		/// @param object Bumper model.
		/// @param dir Bumper direction.
		/// @param t Animation progress.
		/// @note Idle state corresponds to `t = 1`.
		void animate(Object& object, Direction dir, float t) {
			// calculate bumper bump
			float bump = (1.0f - t) * size;

			// calculate bumper shade colors
			float shades[4] {
				lerpf(6  / 32.0f, 1.0f, t),
				lerpf(8  / 32.0f, 1.0f, t),
				lerpf(36 / 32.0f, 1.0f, t),
				lerpf(27 / 32.0f, 1.0f, t),
			};

			// calculate bumper midpoint
			glm::vec2 dv = directionVec(dir) * (0.5f + bump);
			glm::vec3 mid = glm::vec3(dv.x, 0.0f, dv.y);

			// update midpoint vertices
			for (GLuint i = 2; i < 12; i += 3)
				object.vertices()[i].pos = mid;

			// triangle update order
			const GLuint ordering[2][4] {
				{ 0, 1, 2, 3 },
				{ 0, 3, 2, 1 },
			};

			// update vertex colors
			for (GLuint t = 0; t < 4; t++) {
				for (GLuint i = 0; i < 3; i++)
					object.vertices()[t * 3 + i].col = glm::vec3(shades[ordering[!(dir >> 1)][t]]);
			};

			// update geometry
			object.update();
		};
	};
};