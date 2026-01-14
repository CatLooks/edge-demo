#pragma once

/// Level related namespace.
namespace Level {
	/// Dark cube spawner object.
	struct DarkCube {
		/// Spawner data.
		const Bytes::DarkCube* root;

		/// Whether the spawner is active.
		bool active;

		/// Structure constructor.
		/// @param data Spawner data.
		DarkCube (const Bytes::DarkCube* data): root(data), active(true) {};
	};
};