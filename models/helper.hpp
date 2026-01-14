#pragma once

/// Model generation namespace.
namespace Models {
	/// Helper modeling.
	namespace Helper {
		/// Helper size.
		const float size = 11 / 32.0f;

		/// Generates a helper model.
		/// @param object Helper model.
		void generate(Object& object) {
			// helper vertices
			Vertex buffer[4] {
				Vertex({-size, offset * 0.5f - 0.5f,  0.0f}, Textures::Mask::helper.coords[0]),
				Vertex({ 0.0f, offset * 0.5f - 0.5f,  size}, Textures::Mask::helper.coords[1]),
				Vertex({ 0.0f, offset * 0.5f - 0.5f, -size}, Textures::Mask::helper.coords[2]),
				Vertex({ size, offset * 0.5f - 0.5f,  0.0f}, Textures::Mask::helper.coords[3]),
			};

			// update geometry
			object.set(buffer, lengthof(buffer));
			object.indexQuads();
			object.update();
			object.rotZ = M_PI_2;
		};
	};
};