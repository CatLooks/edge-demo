#pragma once

/// Texture map namespace.
namespace Textures {
	/// Mask texture maps.
	namespace Mask {
		/// Mask texture size.
		const glm::vec2 size = {256.0f, 256.0f};

		/// Full square white texture mapping.
		const TexMap whiteF = TexMap { { 32 / size.x, 16 / size.y }, { 96 / size.x, 80 / size.y } };
		/// Half square white texture mapping.
		const TexMap whiteH = TexMap { { 32 / size.x, 80 / size.y }, { 96 / size.x, 112 / size.y } };

		/// Fully white texture mapping.
		const TexMap white = TexMap::face(2, 0, {1.0f / 4, 1.0f / 4}, { 24 / size.x, 24 / size.y });

		/// Shadow volume texture map.
		const TexVol volume = {
			.sur = {
				TexMap::face(3, -1, {1.0f / 4, 1.0f / 4}, {24 / size.x, 24 / size.y}),
				TexMap::face(3,  4, {1.0f / 4, 1.0f / 4}, {24 / size.x, 24 / size.y})
			},
			.z = TexMap::face(7, -4, {1.0f / 8, 1.0f / 4}, {(15 + 1.0f / 4096) / size.x, 0.0f}, {1.0f, 8.0f}),
			.x = TexMap::face(7, -4, {1.0f / 8, 1.0f / 4}, {(15 + 1.0f / 4096) / size.x, 0.0f}, {1.0f, 8.0f})
		};

		/// Helper texture.
		const TexMap helper = TexMap::face(0, 3, {1.0f / 4, 1.0f / 4}, {1.0f / size.x, 1.0f / size.y});

		/// Resizer texture.
		const TexMap resizer = TexMap::face(1, 3, {1.0f / 4, 1.0f / 4}, {1.0f / size.x, 1.0f / size.y});
	};
};