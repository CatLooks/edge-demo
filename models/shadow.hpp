#pragma once

/// Model generation namespace.
namespace Models {
	/// Shadow modeling.
	namespace Shadow {
		// import shadow volume
		using Textures::Mask::volume;

		/// Shadow normalizing offset.
		const glm::vec3 offset = glm::vec3(Models::offset, -Models::offset, Models::offset);

		/// Generates a shadow model.
		/// @param object Shadow model.
		/// @param min Shadow source minimum coordinates.
		/// @param max Shadow source maximum coordinates.
		/// @param ymin Shadow minimum Y-coordinate.
		/// @param ymax Shadow minimum Y-coordinate.
		/// @param dynamic Whether the shadow belongs to a hangable object.
		void generate(Object& object, glm::vec2 min, glm::vec2 max, float ymin, bool dynamic) {
			// shadow dimensions
			glm::vec3 smin = glm::vec3(min.x, ymin , min.y) + offset;
			glm::vec3 smax = glm::vec3(max.x, min.y, max.y) - offset;

			// generate vertices
			if (dynamic) {
				Vertex buffer[48];
				volume.generate(buffer, smin, smax);
				volume.generate(buffer + 24, glm::vec3(0.0f), glm::vec3(0.0f));
				object.set(buffer, lengthof(buffer));
			} else {
				Vertex buffer[24];
				volume.generate(buffer, smin, smax);
				object.set(buffer, lengthof(buffer));
			};
			object.indexQuads();
			object.update();
			object.idCount = 36;
		};

		/// Updates a shadow model.
		/// @param object Shadow model.
		/// @param min Shadow source minimum coordinates.
		/// @param max Shadow source maximum coordinates.
		/// @param ymin Shadow minimum Y-coordinate.
		/// @param ymax Shadow minimum Y-coordinate.
		void update(Object& object, glm::vec2 min, glm::vec2 max, float ymin, float ymax) {
			// shadow dimensions
			glm::vec3 smin = glm::vec3(min.x, ymin, min.y) + offset;
			glm::vec3 smax = glm::vec3(max.x, ymax, max.y) - offset;

			// update shadow vertices
			for (GLuint f = 0; f < 6; f++) {
				for (GLuint i = 0; i < 4; i++)
					object.vertices()[f * 4 + i].setPos(vertID(smin, smax, faces[f][i]));
			};
			object.idCount = 36;
		};

		/// Updates a wall shadow model.
		/// @param object Shadow model.
		/// @param min Shadow source minimum coordinates.
		/// @param max Shadow source maximum coordinates.
		/// @param ymin Shadow minimum Y-coordinate.
		/// @param ymax Shadow minimum Y-coordinate.
		/// @param cube Cube size.
		/// @param lift Shadow elevation.
		/// @param dir Hanging direction.
		/// @note Updated shadow must be dynamic.
		void wall(Object& object, glm::vec2 min, glm::vec2 max, float ymin, float ymax, float cube, float lift, Direction dir) {
			// shadow dimensions
			glm::vec3 smin = glm::vec3(min.x, ymin + lift, min.y);
			glm::vec3 smax = glm::vec3(max.x, ymax, max.y);

			// normalize shadow dimensions
			switch (dir) {
				case East:
					smin += glm::vec3(-Models::offset, -Models::offset,  Models::offset);
					smax -= glm::vec3( Models::offset, -Models::offset,  Models::offset);
					break;
				case West:
					smin += glm::vec3( Models::offset, -Models::offset,  Models::offset);
					smax -= glm::vec3(-Models::offset, -Models::offset,  Models::offset);
					break;
				case North:
					smin += glm::vec3( Models::offset, -Models::offset, -Models::offset);
					smax -= glm::vec3( Models::offset, -Models::offset,  Models::offset);
					break;
				case South:
					smin += glm::vec3( Models::offset, -Models::offset, -Models::offset);
					smax -= glm::vec3( Models::offset, -Models::offset,  Models::offset);
					break;
				default: break;
			};

			// shadow base vertices
			const glm::vec3 verts[4] {
				vertID(smin, smax, faces[5][0]),
				vertID(smin, smax, faces[5][1]),
				vertID(smin, smax, faces[5][2]),
				vertID(smin, smax, faces[5][3]),
			};

			// shadow tip dimensions
			glm::vec3 tmin, tmax;
			switch (dir) {
				case East:
					tmin = verts[0];
					tmax = verts[1] + glm::vec3(Models::offset * 2, cube, 0.0f);
					break;
				case West:
					tmin = verts[2] - glm::vec3(Models::offset * 2, 0.0f, 0.0f);
					tmax = verts[3] + glm::vec3(0.0f, cube, 0.0f);
					break;
				case North:
					tmin = verts[0];
					tmax = verts[2] + glm::vec3(0.0f, cube, Models::offset * 2);
					break;
				case South:
					tmin = verts[1] - glm::vec3(0.0f, 0.0f, Models::offset * 2);
					tmax = verts[3] + glm::vec3(0.0f, cube, 0.0f);
					break;
				default: break;
			};

			// update shadow vertices
			for (GLuint f = 0; f < 6; f++) {
				for (GLuint i = 0; i < 4; i++)
					object.vertices()[f * 4 + i].setPos(vertID(smin, smax, faces[f][i]));
			};

			// update shadow tip vertices
			for (GLuint f = 0; f < 6; f++) {
				for (GLuint i = 0; i < 4; i++)
					object.vertices()[24 + f * 4 + i].setPos(vertID(tmin, tmax, faces[f][i]));
			};
			object.idCount = 72;
		};
	};
};