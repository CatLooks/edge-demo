#pragma once

/// Model generation namespace.
namespace Models {
	/// Resizer modeling.
	namespace Resizer {
		/// Minimal resizer size.
		const float minSize = 0.25f;

		/// Generates a resizer model.
		/// @param object Resizer model.
		void generate(Object& object) {
			// helper default size
			const float size = 0.5f;

			// helper vertices
			Vertex buffer[4] {
				Vertex({-size, offset * 0.5f - 0.5f, -size}, Textures::Mask::resizer.coords[0]),
				Vertex({-size, offset * 0.5f - 0.5f,  size}, Textures::Mask::resizer.coords[1]),
				Vertex({ size, offset * 0.5f - 0.5f, -size}, Textures::Mask::resizer.coords[2]),
				Vertex({ size, offset * 0.5f - 0.5f,  size}, Textures::Mask::resizer.coords[3]),
			};

			// update geometry
			object.set(buffer, lengthof(buffer));
			object.indexQuads();
		};

		/// Animates a resizer model (inward resizing).
		/// @param object Resizer model.
		void animateIn(Object& object) {
			uint8_t timer = Timers::mid.value();

			// get resizer colors
			glm::vec3 colorA = Rainbow::resizerA();
			glm::vec3 colorB = Rainbow::resizerB();

			// update resizer colors
			object.vertices()[0].col = colorA;
			object.vertices()[1].col = colorA;
			object.vertices()[2].col = colorB;
			object.vertices()[3].col = colorB;
			object.update();

			// update resizer alpha
			if (timer < 0xC0)
				object.alpha = 1.0f;
			else if (timer < 0xD0)
				object.alpha = lerpf(1.0f, 0.0f, (timer - 0xC0) / 16.0f);
			else if (timer < 0xF0)
				object.alpha = 0.0f;
			else
				object.alpha = lerpf(0.0f, 1.0f, (timer - 0xF0) / 16.0f);

			// calculate resizer size
			float scale;
			if (timer < 0xC0)
				scale = lerpf(1.0f, minSize, timer / 192.0f);
			else if (timer < 0xE0)
				scale = minSize;
			else if (timer < 0xF4)
				scale = lerpf(minSize, 1.0f, (timer - 0xE0) / 20.0f);
			else
				scale = 1.0f;

			// set resizer size
			object.scale.x = scale;
			object.scale.z = scale;
		};

		/// Animates a resizer model (outward resizing).
		/// @param object Resizer model.
		void animateOut(Object& object) {
			uint8_t timer = Timers::mid.value();

			// get resizer colors
			glm::vec3 colorA = Rainbow::resizerA();
			glm::vec3 colorB = Rainbow::resizerB();

			// update resizer colors
			object.vertices()[0].col = colorA;
			object.vertices()[1].col = colorA;
			object.vertices()[2].col = colorB;
			object.vertices()[3].col = colorB;
			object.update();

			// update resizer alpha
			if (timer < 0xC0)
				object.alpha = 1.0f;
			else if (timer < 0xD0)
				object.alpha = lerpf(1.0f, 0.0f, (timer - 0xC0) / 16.0f);
			else if (timer < 0xF0)
				object.alpha = 0.0f;
			else
				object.alpha = lerpf(0.0f, 1.0f, (timer - 0xF0) / 16.0f);

			// calculate resizer size
			float scale;
			if (timer < 0xC0)
				scale = lerpf(minSize, 1.0f, timer / 192.0f);
			else if (timer < 0xE0)
				scale = 1.0f;
			else if (timer < 0xF4)
				scale = lerpf(1.0f, minSize, (timer - 0xE0) / 20.0f);
			else
				scale = minSize;

			// set resizer size
			object.scale.x = scale;
			object.scale.z = scale;
		};
	};
};