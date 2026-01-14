#pragma once

/// User interface namespace.
namespace UI {
	/// import texture map
	using Textures::TexMap;

	/// Rank image display.
	class RankImage : public Element {
		public:
		/// Displayed rank.
		Data::Rank rank;

		/// Displayed rank texture maps.
		TexMap rainmap;
		TexMap darkmap;

		/// Image alpha.
		float alpha = 1.0f;

		/// Constructor.
		/// @param rank Displayed rank.
		/// @param size Image size.
		RankImage (Data::Rank rank, unsigned int size): rank(rank) {
			box = { 0.5ps - 0.5ws, 0.5ps - 0.5ws, Dim::pix(size), Dim::pix(size) };
			switch (rank) {
				case Data::S:
					rainmap = TexMap::face(0, 0, glm::vec3(1.0f / 4), glm::vec3(0.0f));
					darkmap = TexMap::face(0, 2, glm::vec3(1.0f / 4), glm::vec3(0.0f));
					break;
				case Data::A:
					rainmap = TexMap::face(1, 0, glm::vec3(1.0f / 4), glm::vec3(0.0f));
					darkmap = TexMap::face(1, 2, glm::vec3(1.0f / 4), glm::vec3(0.0f));
					break;
				case Data::B:
					rainmap = TexMap::face(2, 0, glm::vec3(1.0f / 4), glm::vec3(0.0f));
					darkmap = TexMap::face(2, 2, glm::vec3(1.0f / 4), glm::vec3(0.0f));
					break;
				case Data::C:
					rainmap = TexMap::face(3, 0, glm::vec3(1.0f / 4), glm::vec3(0.0f));
					darkmap = TexMap::face(3, 2, glm::vec3(1.0f / 4), glm::vec3(0.0f));
					break;
				case Data::D:
					rainmap = TexMap::face(0, 1, glm::vec3(1.0f / 4), glm::vec3(0.0f));
					darkmap = TexMap::face(0, 3, glm::vec3(1.0f / 4), glm::vec3(0.0f));
					break;
				default:
					rainmap = TexMap::face(1, 1, glm::vec3(1.0f / 4), glm::vec3(0.0f));
					darkmap = TexMap::face(1, 3, glm::vec3(1.0f / 4), glm::vec3(0.0f));
					break;
			};
		};

		protected:
		/// Draws the rank image.
		/// @param self Bounding box.
		void drawSelf(sf::IntRect self) const override {
			Assets::rank.bind();

			// draw the shadow
			sprite().pos = sf::Vector2i(self.left, self.top);
			sprite().size = sf::Vector2i(self.width, self.height);
			sprite().color = glm::vec3(1.0f);
			sprite().alpha = alpha;
			sprite().set(darkmap);
			sprite().draw();

			// draw the rainbow
			sprite().color = rank == Data::S ? Rainbow::textFast(0xE8) : glm::vec3(0.3f);
			sprite().set(rainmap);
			sprite().draw();
		};
	};
};