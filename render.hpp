#pragma once

/// Model generation namespace.
namespace Models {
	/// Level color theme enumeration.
	enum Theme: uint8_t {
		White,
		Gray,
		Black,
		Blue
	};

	/// Returns a string representation of a theme.
	/// @param theme Used theme.
	const char* themeRepr(Theme theme) {
		const char* table[4] {
			"White", "Gray", "Black", "Blue"
		};
		return (theme >= White && theme <= Blue) ? table[theme] : "?";
	};

	/// Mixes 2 vectors.
	/// @param min Negative vector.
	/// @param max Positive vector.
	/// @param ID Vertex selector.
	/// @return Mixed vector.
	/// @note Bits '1' in ID toggle values from `max` vector.
	/// Bit `0` - Z axis.
	/// Bit `1` - Y axis.
	/// Bit `2` - X axis.
	glm::vec3 vertID(glm::vec3 min, glm::vec3 max, uint8_t ID) {
		switch (ID) {
			case 0b000: return { min.x, min.y, min.z };
			case 0b001: return { min.x, min.y, max.z };
			case 0b010: return { min.x, max.y, min.z };
			case 0b011: return { min.x, max.y, max.z };
			case 0b100: return { max.x, min.y, min.z };
			case 0b101: return { max.x, min.y, max.z };
			case 0b110: return { max.x, max.y, min.z };
			case 0b111: return { max.x, max.y, max.z };
			default:    return glm::vec3(0.0f);
		};
	};

	/// Cube face vertex IDs.
	const GLuint faces[6][4] {
		{ 0, 1, 2, 3 }, { 5, 4, 7, 6 }, // X-, X+
		{ 4, 0, 6, 2 }, { 1, 5, 3, 7 }, // Z-, Z+
		{ 1, 0, 5, 4 }, { 2, 3, 6, 7 }, // Y-, Y+
	};

	/// Cube face vertex IDs.
	const GLuint invfaces[6][4] {
		{ 1, 0, 3, 2 }, { 4, 5, 6, 7 }, // X-, X+
		{ 0, 4, 2, 6 }, { 5, 1, 7, 3 }, // Z-, Z+
		{ 0, 1, 4, 5 }, { 3, 2, 7, 6 }, // Y-, Y+
	};

	/// Texture over texture distance.
	const float offset = 1.0f / 512;

	/// Texture over top texture distance.
	const glm::vec3 offsetY = glm::vec3(0.0f, offset, 0.0f);
	/// Texture over Z-axis side texture distance.
	const glm::vec3 offsetZ = glm::vec3(0.0f, 0.0f, offset);
	/// Texture over X-axis side texture distance.
	const glm::vec3 offsetX = glm::vec3(offset, 0.0f, 0.0f);

	/// Volume offset distance.
	const glm::vec3 offsetXYZ = glm::vec3(offset);
};

/// Texture map namespace.
namespace Textures {
	// import model themes
	using Models::Theme::White;
	using Models::Theme::Gray ;
	using Models::Theme::Black;
	using Models::Theme::Blue ;

	// import faces
	using Models::faces;
	using Models::invfaces;

	/// Texture UV mappings.
	struct TexMap {
		glm::vec2 coords[4];

		/// Default empty constructor.
		TexMap () {};
		/// Default vertex constructor.
		TexMap (glm::vec2 bl, glm::vec2 br, glm::vec2 tl, glm::vec2 tr)
			: coords{ bl, br, tl, tr } {};
		/// Default rectangle constructor.
		TexMap (glm::vec2 min, glm::vec2 max)
			: coords{ {min.x, max.y}, max, min, {max.x, min.y} } {};

		/// Monotone face texture.
		/// @param x Face X coordinate.
		/// @param y Face Y coordinate.
		/// @param mult Coordinate multipliers.
		/// @param norm Coordinate normalizers.
		/// @param size Texture size.
		/// @return Texture mapping.
		static TexMap face(GLint x, GLint y, glm::vec2 mult, glm::vec2 norm, glm::vec2 size = glm::vec2(1.0f)) {
			return TexMap(
				{
					x * mult.x + norm.x,
					y * mult.y + norm.y
				}, {
					(x + size.x) * mult.x - norm.y,
					(y + size.y) * mult.y - norm.y
				}
			);
		};

		/// Column face texture.
		/// @param x Column X coordinate.
		/// @param y Column Y coordinate.
		/// @param h Column height.
		/// @param mult Coordinate multipliers.
		/// @param norm Coordinate normalizers.
		/// @return Texture mapping.
		static TexMap column(GLint x, float y, float h, glm::vec2 mult, glm::vec2 norm) {
			return TexMap(
				{ (x + 0) * mult.x + norm.x, (y    ) * mult.y },
				{ (x + 1) * mult.x - norm.x, (y + h) * mult.y }
			);
		};

		/// Returns center of texture rectangle.
		glm::vec2 center() const {
			return (coords[0] + coords[3]) * 0.5f;
		};
	};

	/// Volume texture mappings.
	struct TexVol {
		TexMap sur[2]; /// Surfaces (bottom and top).
		TexMap z;      /// Y-axis sides.
		TexMap x;      /// X-axis sides.

		/// Generates vertices in specified region.
		/// @param verts Vertex buffer (24 vertices).
		/// @param min Minimum coordinates.
		/// @param max Maximum coordinates.
		void generate(Vertex* verts, glm::vec3 min, glm::vec3 max) const {
			// generate X-axis sides
			for (GLuint f = 0; f < 2; f++) {
				for (GLuint i = 0; i < 4; i++)
					verts[f * 4 + i] = Vertex(Models::vertID(min, max, faces[f][i]), x.coords[i]);
			};
			// generate Z-axis sides
			for (GLuint f = 2; f < 4; f++) {
				for (GLuint i = 0; i < 4; i++)
					verts[f * 4 + i] = Vertex(Models::vertID(min, max, faces[f][i]), z.coords[i]);
			};
			// generate Y-axis sides
			for (GLuint f = 4; f < 6; f++) {
				for (GLuint i = 0; i < 4; i++)
					verts[f * 4 + i] = Vertex(Models::vertID(min, max, faces[f][i]), sur[f & 1].coords[i]);
			};
		};
	};
};

// import texture mappings
#include "textures/terrain.hpp"
#include "textures/platform.hpp"
#include "textures/mask.hpp"

// import model generation units
#include "models/space.hpp"
#include "models/shadow.hpp"
#include "models/static.hpp"
#include "models/hint.hpp"
#include "models/moving.hpp"
#include "models/falling.hpp"
#include "models/bumper.hpp"
#include "models/button.hpp"
#include "models/helper.hpp"
#include "models/resizer.hpp"
#include "models/prism.hpp"
#include "models/cube.hpp"
#include "models/exit.hpp"
#include "models/particles.hpp"

// import model instances
#include "models/instances.hpp"