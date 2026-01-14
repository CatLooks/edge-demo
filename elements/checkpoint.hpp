#pragma once

/// Level related namespace.
namespace Level {
	/// Checkpoint object.
	struct Checkpoint {
		/// Checkpoint data.
		const Bytes::Checkpoint* root;

		/// Whether the checkpoint is active.
		bool active;

		/// Structure constructor.
		/// @param data Checkpoint data.
		Checkpoint (const Bytes::Checkpoint* data): root(data), active(true) {};
	};
};