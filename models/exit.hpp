#pragma once

/// Cube model generation namespace.
namespace Models {
	/// Exit modeling.
	namespace Exit {
		/// Generates an exit model.
		/// @param object Exit model.
		void generate(Object& object) {
			// generate vertices
			Vertex buffer[12] {
				// inner face vertices
				Vertex({-0.5f, -0.5f, -0.5f}, Textures::Mask::white.coords[0]),
				Vertex({-0.5f, -0.5f,  0.5f}, Textures::Mask::white.coords[1]),
				Vertex({ 0.5f, -0.5f, -0.5f}, Textures::Mask::white.coords[2]),
				Vertex({ 0.5f, -0.5f,  0.5f}, Textures::Mask::white.coords[3]),

				// outer face inner vertices
				Vertex({-0.5f, -0.5f, -0.5f}, Textures::Mask::white.coords[0]),
				Vertex({-0.5f, -0.5f,  0.5f}, Textures::Mask::white.coords[1]),
				Vertex({ 0.5f, -0.5f, -0.5f}, Textures::Mask::white.coords[1]),
				Vertex({ 0.5f, -0.5f,  0.5f}, Textures::Mask::white.coords[0]),

				// outer face outer vertices
				Vertex({-1.5f, -0.5f, -1.5f}, Textures::Mask::white.coords[2]),
				Vertex({-1.5f, -0.5f,  1.5f}, Textures::Mask::white.coords[3]),
				Vertex({ 1.5f, -0.5f, -1.5f}, Textures::Mask::white.coords[3]),
				Vertex({ 1.5f, -0.5f,  1.5f}, Textures::Mask::white.coords[2]),
			};

			// generate indices
			std::vector<GLuint> indices;
			Object::pushQuad(indices, 0x0, 0x1, 0x2, 0x3);
			Object::pushQuad(indices, 0x8, 0x9, 0x4, 0x5);
			Object::pushQuad(indices, 0x9, 0xB, 0x5, 0x7);
			Object::pushQuad(indices, 0xB, 0xA, 0x7, 0x6);
			Object::pushQuad(indices, 0xA, 0x8, 0x6, 0x4);

			// update geometry
			object.set(buffer, lengthof(buffer));
			object.index(indices);
		};

		/// Updates an exit model.
		/// @param object Exit model.
		void update(Object& object) {
			// update inner face
			for (GLuint i = 0; i < 4; i++)
				object.vertices()[i].col = Rainbow::exitA();

			// update outer faces
			for (GLuint i = 4; i < 12; i++)
				object.vertices()[i].col = Rainbow::exitB();
			object.update();
		};
	};
};