#pragma once

/// Model generation namespace.
namespace Models {
	/// Small grid size.
	const float small = 1.0f / 3;

	/// Prism modeling.
	namespace Prism {
		/// Prism shadow height.
		const float shadowZ = -2.0f;

		/// Generates a prism.
		/// @param object Prism model.
		/// @param shadow Prism shadow model.
		void generate(Object& object, Object& shadow) {
			Vertex buffer[24];

			// cube dimensions
			glm::vec3 min = glm::vec3(-0.5f * small);
			glm::vec3 max = glm::vec3( 0.5f * small);

			// generate cube vertices
			for (GLuint f = 0; f < 6; f++) {
				for (GLuint i = 0; i < 4; i++)
					buffer[f * 4 + i] = Vertex(vertID(min, max, faces[f][i]), Textures::Mask::white.coords[i]);
			};

			// update geometry
			object.set(buffer, lengthof(buffer));
			object.indexQuads();

			// generate shadow
			Shadow::generate(shadow, min, max, shadowZ, false);
		};

		/// Sets cube X-axis sides' color.
		/// @param object Cube model object.
		/// @param color Cube face color.
		void colorX(Object& object, glm::vec3 color) {
			for (GLuint f = 0; f < 2; f++) {
				for (GLuint i = 0; i < 4; i++)
					object.vertices()[f * 4 + i].setCol(color);
			};
		};

		/// Sets cube Y-axis sides' color.
		/// @param object Cube model object.
		/// @param color Cube face color.
		void colorY(Object& object, glm::vec3 color) {
			for (GLuint f = 4; f < 6; f++) {
				for (GLuint i = 0; i < 4; i++)
					object.vertices()[f * 4 + i].setCol(color);
			};
		};

		/// Sets cube Z-axis sides' color.
		/// @param object Cube model object.
		/// @param color Cube face color.
		void colorZ(Object& object, glm::vec3 color) {
			for (GLuint f = 2; f < 4; f++) {
				for (GLuint i = 0; i < 4; i++)
					object.vertices()[f * 4 + i].setCol(color);
			};
		};
	};
};