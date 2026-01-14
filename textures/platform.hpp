#pragma once

/// Texture map namespace.
namespace Textures {
	/// Platform texture maps.
	namespace Platform {
		/// Falling platform texture multiplier.
		const glm::vec2 mult = glm::vec2(1.0f / 8, 1.0f / 8);

		/// Falling platform texture multiplier.
		const glm::vec2 norm = glm::vec2(1.0f / 128, 1.0f / 128);

		/// Falling platform surface texture multiplier.
		const glm::vec2 surN = norm * (1.0f - 1.0f / 4);

		/// Falling platform texture maps.
		const TexVol fallingMaps[4] {
			[White] = { .sur = { TexMap::face(0, 1, mult, surN), TexMap::face(0, 0, mult, surN) }, .z = TexMap::face(0, 2, mult, norm), .x = TexMap::face(0, 3, mult, norm) },
			[Gray ] = { .sur = { TexMap::face(1, 1, mult, surN), TexMap::face(1, 0, mult, surN) }, .z = TexMap::face(1, 2, mult, norm), .x = TexMap::face(1, 3, mult, norm) },
			[Black] = { .sur = { TexMap::face(2, 1, mult, surN), TexMap::face(2, 0, mult, surN) }, .z = TexMap::face(2, 2, mult, norm), .x = TexMap::face(2, 3, mult, norm) },
			[Blue ] = { .sur = { TexMap::face(3, 1, mult, surN), TexMap::face(3, 0, mult, surN) }, .z = TexMap::face(3, 2, mult, norm), .x = TexMap::face(3, 3, mult, norm) },
		};

		/// Button volume texture map.
		const TexVol buttonMap = {
			.sur = {
				TexMap::face(4, 1, mult, norm),
				TexMap::face(4, 0, mult, norm)
			},
			.z = TexMap::face(4, 2, mult, norm),
			.x = TexMap::face(4, 3, mult, norm)
		};
	};
};