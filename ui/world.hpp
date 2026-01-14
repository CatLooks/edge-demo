#pragma once

/// User interface namespace.
namespace UI {
	/// World level select menu element.
	class World : public Element {
		protected:
		/// Worldmap route data.
		const std::vector<Bytes::Route>* routes;
		/// Amount of icons per row.
		uint32_t iconsPerRow = 1;
		/// Previously selected level index.
		size_t selectPrev = -1ULL;
		/// Average rank label.
		Text* rText;
		/// Prism count label.
		Text* pText;
		/// World stat values.
		Data::Rank s_rank = Data::N;
		uint32_t s_pttl = 0;
		uint32_t s_pcol = 0;

		public:
		/// Level info panel.
		LevelInfo* m_info;
		/// Menu panel.
		Panel* m_panel;
		/// Menu content container.
		Element* m_base;
		/// Controlled world map.
		Level::Level* level = NULL;
		/// Menu content.
		std::vector<Icon*> icons;
		/// Selected level index.
		size_t selected = 0;
		/// Level info display request.
		bool infoReq = false;
		/// Level start request.
		bool playReq = false;

		/// Menu constructor.
		/// @param routes Route collection data.
		World (const Data::World& world): routes(&world.routedata.routes), m_info(NULL) {
			box = { 0px, 0px, 1ps, 1ps };
			active = false;

			// create dummy level info panel
			m_info = new LevelInfo(NULL);

			// create level select menu
			m_panel = new Panel;
			m_panel->box = { -1ws, 0ps, 384px, 1ps };
			m_panel->color = RGB(0x3B3B3B);
			add(m_panel);

			// create menu container
			m_base = new Element;
			m_base->box = { 0px, -1ps, 1ps, 1ps };
			m_panel->add(m_base);
			{
				// create menu label
				Panel* tPanel = new Panel;
				tPanel->box = { 8px, 8px, 1ps - 16px, 64px };
				tPanel->border = RGB(0x3B3B3B);
				tPanel->thickness = 1;
				tPanel->rainbow = true;
				m_base->add(tPanel);
				{
					UI::Text* text = new Text;
					text->font = 4;
					text->box.pos = { 8px, 0.5ps - 0.5ws + Dim::pix(text->font) };
					text->setText("\2Levels");
					tPanel->add(text);
				};

				// create average stats label
				Panel* aPanel = new Panel;
				aPanel->box = { 8px, 1ps - 64px, 1ps - 16px, 56px };
				aPanel->border = RGB(0x3B3B3B);
				aPanel->thickness = 1;
				m_base->add(aPanel);
				{
					// create average rank label
					rText = new Text;
					rText->font = 3;
					rText->box.pos = { 8px, 0.5ps - 0.5ws + Dim::pix(rText->font) };
					aPanel->add(rText);

					// create prism stats label
					pText = new Text;
					pText->font = 3;
					pText->box.pos = { 1ps - 1ws - 8px, 0.5ps - 0.5ws + Dim::pix(pText->font) };
					aPanel->add(pText);

					// request initial stats update
					updateStats(world);
				};
			};

			// create level menu
			Panel* menu = new Panel;
			menu->box = { 8px, 72px, 1ps - 16px, 1ps - 72px - 64px };
			menu->border = RGB(0x3B3B3B);
			menu->thickness = 1;
			m_base->add(menu);
			{
				// menu panel padding
				sf::Vector2i padding = { 12, 12 };

				// icon layout cursor
				sf::Vector2i cursor;

				// create level icons
				uint32_t iconCount = 0;
				for (uint32_t i = 0; i < routes->size(); i++) {
					// generate icon
					Icon* icon = new Icon(i + 1, false, false);
					icon->box = {
						Dim::pix(padding.x + cursor.x),
						Dim::pix(padding.y + cursor.y),
						64px, 64px
					};
					icon->eventCall(MouseDown, [=](Element&, const EventData&) -> bool {
						// check if level is unlocked
						if (icons[i]->locked) return true;

						// check if click was on selected level
						if (selected == i) {
							// request level start
							playReq = true;
							Assets::sfx(Assets::Start);
						} else {
							// select the level
							if (selected != i) {
								selected = i;
								moveCube();
							};
							Assets::sfx(Assets::Select);
						};
						return true;
					});
					icons.push_back(icon);
					menu->add(icon);
					iconCount++;

					// update cursor
					cursor.x += 70;
					if (cursor.x >= (m_panel->box.size.x - menu->box.pos.x * 2 - 64px).off - padding.x) {
						cursor.x = 0;
						cursor.y += 70;

						// update max icons per row count
						if (iconCount > iconsPerRow)
							iconsPerRow = iconCount;
						iconCount = 0;
					};
				};
			};

			// menu browsing handler
			eventCall(KeyDown, [=](Element&, const EventData& data) {
				// select next level
				if (data.key == sf::Keyboard::Right) {
					// find next unlocked level
					size_t prev = selected;
					while (true) {
						if (++selected >= routes->size()) {
							selected = prev;
							break;
						};
						if (!icons[selected]->locked) break;
					};

					// move cube
					if (selected != prev) moveCube();
					Assets::sfx(Assets::Select);
					return true;
				};

				// select previous level
				if (data.key == sf::Keyboard::Left) {
					// find next unlocked level
					size_t prev = selected;
					while (true) {
						if (--selected >= routes->size()) {
							selected = prev;
							break;
						};
						if (!icons[selected]->locked) break;
					};

					// move cube
					if (selected != prev) moveCube();
					Assets::sfx(Assets::Select);
					return true;
				};

				// select level below
				if (data.key == sf::Keyboard::Down) {
					// find next unlocked level
					size_t prev = selected;
					while (true) {
						if ((selected += iconsPerRow) >= routes->size()) {
							selected = prev;
							break;
						};
						if (!icons[selected]->locked) break;
					};

					// move cube
					if (selected != prev) moveCube();
					Assets::sfx(Assets::Select);
					return true;
				};

				// select level above
				if (data.key == sf::Keyboard::Up) {
					// find next unlocked level
					size_t prev = selected;
					while (true) {
						if ((selected -= iconsPerRow) >= routes->size()) {
							selected = prev;
							break;
						};
						if (!icons[selected]->locked) break;
					};

					// move cube
					if (selected != prev) moveCube();
					Assets::sfx(Assets::Select);
					return true;
				};

				// select level
				if (data.key == sf::Keyboard::Enter) {
					icons[selected]->forceEvent(MouseDown, EventData());
					return true;
				};

				// unknown event
				return false;
			});
		};

		/// Moves the cube to new route.
		void moveCube() {
			infoReq = true;
			if (level) {
				const Bytes::Route& route = routes->at(selected);

				// warp cube
				level->mutePrism = true;
				level->getPlayer().tile = route.pos;

				// apply cube size
				if (level->getPlayer().settings.mini != route.mini) {
					if (route.mini)
						level->getPlayer().shrink();
					else
						level->getPlayer().grow(true);
				};

				// apply background gradient
				Models::Space::set(Models::Grad::fromTheme(route.grad), 0.4f);
			};
		};

		/// Update world stats.
		/// @param world World object.
		void updateStats(const Data::World& world) {
			// request stats update
			const char* avg = world.stats(s_pcol, s_pttl);

			// update label text
			rText->setFmt("\x13\x19Rank: \1\2%s", avg);
			pText->setFmt("\2\6%llu\7/\6%llu\7 \\", s_pcol, s_pttl);
		};

		/// Recolors level icons.
		/// @param world World object.
		void recolorIcons(const Data::World& world) {
			// is previous level available?
			bool prev = true;

			// recolor each icon
			for (size_t i = 0; i < routes->size(); i++) {
				// get level info
				const std::string& file = world.routedata.routes[i].file;
				Bytes::Sign sign = world.findFile(file);
				Data::Record record = world.save.get(sign.ID);

				// check if level info is missing
				if (sign.ID == 0) {
					icons[i]->rain = false;
					icons[i]->locked = true;
				};

				// check if the level is available
				icons[i]->locked = (record.id == 0 && !prev);

				// recolor the icon
				icons[i]->rain = world.merge(sign, record, 0).rank == Data::S;

				// update previous flag
				prev = record.id != 0;
			};
		};

		/// Creates icon switch animations.
		/// @param anims Animator list.
		void emitIconAnims(std::list<std::unique_ptr<Animator>>& anims) {
			// create animation manager update
			updateCall([&](Element&, float) {
				// check for no selection changes
				if (selected == selectPrev)
					return;

				// unselect previous level
				if (selectPrev != -1ULL)
					icons.at(selectPrev)->emitDeselect(anims);

				// select new level
				icons.at(selected)->emitSelect(anims);

				// update selection state
				selectPrev = selected;
			});
		};

		/// Emits entry animation.
		/// @param anims Animator list.
		void emitEntry(std::list<std::unique_ptr<Animator>>& anims) {
			active = true;

			// create panel entry
			AnimDim* anim = new AnimDim;
			anim->setEasing(Easings::sineOut);
			anim->start(&m_panel->box.pos.x, 0.1ps, 0.2f);
			anim->finish([&](AnimDim&) {
				// create content entry
				AnimDim* anim = new AnimDim;
				anim->setEasing(Easings::sineOut);
				anim->start(&m_base->box.pos.y, 0ps, 0.3f);
				anims.push_back(std::unique_ptr<Animator>(anim));
			});
			anims.push_back(std::unique_ptr<Animator>(anim));
		};

		/// Returns a camera offset for level complete menu.
		/// @param camera Target camera.
		glm::vec3 cameraOffset(const Camera& camera) {
			return UI::cameraOffset(camera, m_panel->box.pos.x.par + (float)m_panel->box.size.x.off / Win::size.x);
		};
	};
};