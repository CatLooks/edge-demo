#pragma once

/// Model generation namespace.
namespace Textures {
	/// Terrain textures.
	namespace Terrain {
		/// Terrain texture UV multipliers.
		const glm::vec2 mult = { 1.0f / 16, 1.0f / 8 };

		/// Terrain texture UV normalizers.
		const glm::vec2 norm = { 15.0f / 512, 15.0f / 256 };

		/// Theme shade level texture mapping collection.
		struct TexLevel {
			TexMap surface[2]; /// Checker pattern surfaces.
			TexMap columnX[2]; /// Full & half block X-axis side surface.
			TexMap columnZ[2]; /// Full & half block Z-axis side surface.
			TexMap floor  [2]; /// Full & half block floor surface.

			/// Default empty constructor.
			TexLevel () {};
			/// Default constructor.
			/// @param theme Model theme.
			/// @param shade Shading level.
			TexLevel (Models::Theme theme, uint8_t shade) {
				GLint x = theme * 2;
				GLint s = shade;
				float c = shade * 2.0f;

				// convert undershade
				if (shade == 4)
					{ c = 8.0f; s = 3; }
				// convert overshade
				else if (shade >= 5)
					{ c = -2.0f; s = 0; };

				this->surface[0] = TexMap::face  (x + 8, s      , mult, norm);
				this->surface[1] = TexMap::face  (x + 9, s      , mult, norm);
				this->columnX[0] = TexMap::column(x + 1, c, 2.0f, mult, norm);
				this->columnX[1] = TexMap::column(x + 1, c, 1.0f, mult, norm);
				this->columnZ[0] = TexMap::column(x + 0, c, 2.0f, mult, norm);
				this->columnZ[1] = TexMap::column(x + 0, c, 1.0f, mult, norm);
				this->floor  [0] = TexMap::face  (x + 8, s + 4  , mult, norm);
				this->floor  [1] = TexMap::face  (x + 9, s + 4  , mult, norm);
			};
		};

		/// Theme texture mappings.
		const TexLevel texMaps[4][6] {
			[White] = { TexLevel(White, 0), TexLevel(White, 1), TexLevel(White, 2), TexLevel(White, 3), TexLevel(White, 4), TexLevel(White, 5) },
			[Gray ] = { TexLevel(Gray , 0), TexLevel(Gray , 1), TexLevel(Gray , 2), TexLevel(Gray , 3), TexLevel(Gray , 4), TexLevel(Gray , 5) },
			[Black] = { TexLevel(Black, 0), TexLevel(Black, 1), TexLevel(Black, 2), TexLevel(Black, 3), TexLevel(Black, 4), TexLevel(Black, 5) },
			[Blue ] = { TexLevel(Blue , 0), TexLevel(Blue , 1), TexLevel(Blue , 2), TexLevel(Blue , 3), TexLevel(Blue , 4), TexLevel(Blue , 5) },
		};

		/// White texture map.
		const TexMap white = texMaps[White][0].surface[0];

		/// Cube sides generation settings.
		struct Sides {
			bool xn, xp;
			bool yn, yp;
			bool zn, zp;

			/// Default constructor.
			Sides (): xn(true), xp(true), yn(true), yp(true), zn(true), zp(true) {};
		};

		/// Converts a Z coordinate into a shading level.
		/// @param Z coordinate.
		/// @return Shade number.
		uint8_t shadeZ(uint16_t z) {
			return z < 4 ? (3 - z) : 5;
		};
	};
};