#pragma once

/// User interface namespace.
namespace UI {
	/// Panel element.
	class Panel : public Element {
		public:
		/// Panel color.
		glm::vec3 color = glm::vec3(1.0f);
		/// Border color.
		glm::vec3 border = glm::vec3(0.3f);
		/// Border size.
		int thickness = 0;
		/// Panel alpha.
		float alpha = 1.0f;
		/// Whether to make the panel rainbow.
		bool rainbow = false;

		protected:
		/// Draws the panel.
		/// @param self Bounding box.
		void drawSelf(sf::IntRect self) const override {
			Assets::white.bind();

			// get panel color
			glm::vec3 drawColor = rainbow ? Rainbow::textFast(0x80) : color;

			// draw panel without borders
			if (thickness == 0) {
				sprite().pos = sf::Vector2i(self.left, self.top);
				sprite().size = sf::Vector2i(self.width, self.height);
				sprite().color = drawColor;
				sprite().alpha = alpha;
				sprite().draw();
				return;
			};

			// draw the border
			sprite().pos = sf::Vector2i(self.left, self.top);
			sprite().size = sf::Vector2i(self.width, self.height);
			sprite().color = border;
			sprite().alpha = alpha;
			sprite().draw();

			// draw the panel
			sf::Vector2i offset = { thickness, thickness };
			sprite().pos += offset;
			sprite().size -= offset * 2;
			sprite().color = drawColor;
			sprite().draw();
		};
	};
};