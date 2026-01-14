#pragma once

/// Level related namespace.
namespace Level {
	/// Prism object.
	struct Prism {
		/// Prism data.
		const Bytes::Prism* root;

		/// Whether the prism is dark.
		bool dark;

		/// Structure constructor.
		/// @param data Prism data.
		Prism (const Bytes::Prism* data): root(data), dark(false) {};

		/// Draws the prism.
		/// @return Drawn vertex count.
		size_t draw() {
			Assets::mask.bind();
			return Models::drawPrism(root->pos.gl(), dark);
		};

		/// Draws prism's shadow.
		/// @return Drawn vertex count.
		size_t shadow() {
			Assets::mask.bind();
			return Models::drawPrismShadow(root->pos.gl(), dark);
		};

		/// Checks whether the prism touches a cube.
		/// @param cube Cube collision volume.
		bool in(const vol3x& cube) const {
			vec3x pos = root->pos + vec3x(0.5fx);
			vec3x rad = vec3x(1fx / 6fx);
			vol3x vol = vol3x::at(pos, rad);
			return cube.intersects(vol);
		};

		/// Plays prism collection sound.
		void sfx() const {
			Assets::sfx(dark ? Assets::Dark : Assets::Prism);
		};
	};
};