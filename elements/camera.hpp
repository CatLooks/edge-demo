#pragma once

/// Level related namespace.
namespace Level {
	/// Camera trigger object.
	struct CameraTrigger {
		/// Camera trigger data.
		const Bytes::CameraTrigger* root;

		/// Structure constructor.
		/// @param data Camera trigger data.
		CameraTrigger (const Bytes::CameraTrigger* data): root(data) {};

		/// Returns camera interpolation time.
		float time() const { return convTicks(root->time); };

		/// Returns camera offset for a zoom value.
		glm::vec3 zoom() const { return zooms(root->zoom); };
		static glm::vec3 zooms(uint8_t val) {
			switch (val) {
				case 1: return glm::vec3(3.0f);
				case 2: return glm::vec3(5.0f);
				case 3: return glm::vec3(8.0f);
				default: return glm::vec3(5.0f);
			};
		};
	};
};