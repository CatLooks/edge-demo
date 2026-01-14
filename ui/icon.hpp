#pragma once

/// User interface namespace.
namespace UI {
	/// Level icon element.
	class Icon : public Panel {
		protected:
		Text* text;
		uint16_t no;
		bool selected;

		public:
		/// Whether the icon is locked.
		bool locked;
		/// Whether to shade border with rainbow.
		bool rain;

		/// Icon constructor.
		/// @param no Level number.
		/// @param locked Whether the level is locked.
		/// @param rain Whether the border should be rainbow shaded.
		Icon (uint16_t no, bool locked, bool rain): no(no), selected(false), locked(locked), rain(rain) {
			// setup icon panel
			thickness = 4;

			// add level number
			text = new Text;
			text->font = 3;
			text->box.pos = { 0.5ps - 0.5ws, 0.5ps - 0.5ws + Dim::pix(text->font) };
			text->setFmt("\2%u", no);
			add(text);
		};

		/// Emits an icon selection animation.
		/// @param anims Animator list.
		void emitSelect(std::list<std::unique_ptr<Animator>>& anims) {
			// update text
			text->setFmt("\x19\2\6%u", no);
			selected = true;
			{
				// extend border
				AnimInt* anim = new AnimInt;
				anim->start(&thickness, 6, 0.1f);
				anims.push_back(std::unique_ptr<Animator>(anim));
			};
			{
				// bump text up
				AnimInt* anim = new AnimInt;
				anim->setEasing(Easings::quadOut);
				anim->start(&text->box.pos.y.off, text->font - 8, 0.1f);
				anim->finish([&](AnimInt&) {
					// bump text down
					AnimInt* anim = new AnimInt;
					anim->setEasing(Easings::quadIn);
					anim->start(&text->box.pos.y.off, text->font, 0.1f);
					anims.push_back(std::unique_ptr<Animator>(anim));
				});
				anims.push_back(std::unique_ptr<Animator>(anim));
			};
		};

		/// Emits an icon deselection animation.
		void emitDeselect(std::list<std::unique_ptr<Animator>>& anims) {
			// update text
			text->setFmt("\2%u", no);
			selected = false;
			{
				// extend border
				AnimInt* anim = new AnimInt;
				anim->start(&thickness, 4, 0.1f);
				anims.push_back(std::unique_ptr<Animator>(anim));
			};
		};

		protected:
		/// Draws the level icon.
		/// @param self Bounding box.
		void drawSelf(sf::IntRect self) const override {
			// update colors
			glm::vec3 sBorder = (rain && !locked) ? Rainbow::gui(0x33, 1) : RGB(0x1C1D1F);
			glm::vec3 sColor = locked
				? (selected ? Rainbow::gui(0xB3, 2) : RGB(0x434549))
				: (selected ? Rainbow::gui(0xB3, 2) : RGB(0xF0F0F0));

			// draw panel
			{
				Assets::white.bind();

				// draw the border
				sprite().pos = sf::Vector2i(self.left, self.top);
				sprite().size = sf::Vector2i(self.width, self.height);
				sprite().color = sBorder;
				sprite().alpha = alpha;
				sprite().draw();

				// draw the panel
				sf::Vector2i offset = { thickness, thickness };
				sprite().pos += offset;
				sprite().size -= offset * 2;
				sprite().color = sColor;
				sprite().draw();
			};

			// border angle texture map
			static Textures::TexMap tex = Textures::TexMap::face(3, 1, glm::vec3(1.0f / 4), glm::vec3(0.0f));

			// draw border angle
			Assets::rank.bind();
			sf::Vector2i outline = { thickness, thickness };
			sprite().size = sf::Vector2i(32, 32);
			sprite().pos = sf::Vector2i(self.left + self.width, self.top + self.height)
				- outline - sprite().size;
			sprite().color = sBorder;
			sprite().alpha = alpha;
			sprite().set(tex);
			sprite().draw();
		};
	};
};