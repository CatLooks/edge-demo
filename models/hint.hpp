#pragma once

/// Model generation namespace.
namespace Models {
	/// Hint modeling.
	namespace Hint {
		/// Half of hint shadow size.
		const float size = 3 / 8.0f;

		/// Generates a hint model.
		/// @param object Hint model.
		void generate(Object& object) {
			// hint vertices
			Vertex buffer[4] {
				Vertex({-size, offset * 0.5f - 0.5f, -size}, Textures::Mask::volume.sur[1].coords[0]),
				Vertex({-size, offset * 0.5f - 0.5f,  size}, Textures::Mask::volume.sur[1].coords[1]),
				Vertex({ size, offset * 0.5f - 0.5f, -size}, Textures::Mask::volume.sur[1].coords[2]),
				Vertex({ size, offset * 0.5f - 0.5f,  size}, Textures::Mask::volume.sur[1].coords[3]),
			};

			// update geometry
			object.set(buffer, lengthof(buffer));
			object.indexQuads();
			object.update();
		};
	};
};