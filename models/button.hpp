#pragma once

/// Model generation namespace.
namespace Models {
	/// Button modeling.
	namespace Button {
		/// Button width.
		const float width = 0.5f;

		/// Button height.
		const float height = 0.125f;

		/// Generates a button model.
		/// @param object Button model object.
		/// @param pressed Whether the button is pressed.
		void generate(Object& object, bool pressed) {
			if (!pressed) {
				Vertex buffer[24];

				// button dimensions
				glm::vec3 min = glm::vec3(-width / 2,        - 0.5f, -width / 2);
				glm::vec3 max = glm::vec3( width / 2, height - 0.5f,  width / 2);

				// update geometry
				Textures::Platform::buttonMap.generate(buffer, min, max);
				object.set(buffer, lengthof(buffer));
			} else {
				// hint vertices
				Vertex buffer[4] {
					Vertex({-width / 2, offset * 0.75f - 0.5f, -width / 2}, Textures::Platform::buttonMap.sur[1].coords[0]),
					Vertex({-width / 2, offset * 0.75f - 0.5f,  width / 2}, Textures::Platform::buttonMap.sur[1].coords[1]),
					Vertex({ width / 2, offset * 0.75f - 0.5f, -width / 2}, Textures::Platform::buttonMap.sur[1].coords[2]),
					Vertex({ width / 2, offset * 0.75f - 0.5f,  width / 2}, Textures::Platform::buttonMap.sur[1].coords[3]),
				};

				// update geometry
				object.set(buffer, lengthof(buffer));
			};
			object.indexQuads();
			object.update();
		};

		/// Updates a button model.
		/// @param object Button model object.
		/// @param pressed Whether the button is pressed.
		/*void update(Object& object, bool pressed) {
			// button dimensions
			glm::vec3 min = glm::vec3(-width / 2,                             - 0.5f, -width / 2);
			glm::vec3 max = glm::vec3( width / 2, (pressed ? offset : height) - 0.5f,  width / 2);

			// update geometry
			for (GLuint f = 0; f < 6; f++) {
				for (GLuint i = 0; i < 4; i++)
					object.vertices()[f * 4 + i].setPos(vertID(min, max, faces[f][i]));
			};
		};*/
	};
};