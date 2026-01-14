#pragma once

/// Model generation namespace.
namespace Models {
	/// Falling platform modeling.
	namespace Falling {
		/// Falling platform height.
		const float height = 1.0f / 8;

		/// Generates a falling platform model.
		/// @param object Falling platform model object.
		/// @param theme Falling platform theme.
		void generate(Object& object, Theme theme) {
			Vertex buffer[24];

			// falling platform dimensions
			glm::vec3 min = glm::vec3(-0.5f, 0.5f - height, -0.5f);
			glm::vec3 max = glm::vec3(0.5f);

			// update geometry
			Textures::Platform::fallingMaps[theme].generate(buffer, min, max);
			object.set(buffer, lengthof(buffer));
			object.indexQuads();
			object.update();
		};
	};
};