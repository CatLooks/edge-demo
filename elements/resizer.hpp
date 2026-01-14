#pragma once

/// Level related namespace.
namespace Level {
	/// Resizer object.
	struct Resizer {
		/// Resizer data.
		const Bytes::Resizer* root;

		/// Structure constructor.
		/// @param data Resizer data.
		Resizer (const Bytes::Resizer* data): root(data) {};

		/// Draws the resizer.
		/// @return Drawn vertex count.
		size_t draw() {
			if (root->visible) {
				Assets::mask.bind();
				return Models::drawResizer(root->pos.gl(), root->mode == Bytes::Resizer::Shrink);
			};
			return 0;
		};
	};
};