#pragma once

/// User interface namespace.
namespace UI {
	/// Single edge time text element.
	class Edgetime : public Text {
		protected:
		/// Target level object.
		Level::Level* mLevel = NULL;

		public:
		/// Target cube index.
		size_t mID;

		/// Edge time label constructor.
		/// @param level Level pointer.
		/// @param id Cube index.
		Edgetime (Level::Level* level, size_t id): mLevel(level), mID(id) {
			font = 6;
			updateCall([&](Element&, float) {
				if (mLevel == NULL) return;
				Level::Cube& cube = mLevel->state.cubes[mID];

				// deactivate if no hang time
				auto it = mLevel->hang.find(mID);
				if (!cube.hang || it == mLevel->hang.end()) {
					active = false;
					return;
				};

				// get cube hang time
				float time = it->second;

				// update label text
				int wp = (int)time;
				int dp = (int)(time * 100) % 100;
				setFmt("\2%c%d'%02d", cube.dark ? '\x10' : '\6', wp, dp);

				// update label position
				sf::IntRect rect = mLevel->camera.vertBounds(cube.volume);
				switch (cube.dir) {
					case East: case South:
						box.pos = Dim2::fromVec({ rect.left + rect.width, rect.top });
						break;
					case West: case North:
						box.pos = Dim2::fromVec({ rect.left, rect.top }) + Dim2(-1ws, 0px);
						break;
				};
			});
		};
	};

	/// Hangtime element.
	class Hangtime : public Element {
		public:
		/// Target level object.
		Level::Level* mLevel = NULL;
		/// Edge time label list.
		std::list<Edgetime*> labels;

		/// Hangtime constructor.
		/// @param level Level object pointer.
		Hangtime (Level::Level* level = NULL): mLevel(level) {
			box = { 0px, 0px, 1ps, 1ps };

			// create a text label
			UI::Text* text = new UI::Text;
			text->font = 6;
			text->active = false;
			add(text);

			// setup text update call
			updateCall([&](UI::Element& elem, float delta) {
				// ignore if no level is bound
				if (mLevel == NULL) return;

				// spawn new edgetime labels
				for (auto it = mLevel->hang.begin(); it != mLevel->hang.end(); it++) {
					// check if edgetime is not displayed yet
					bool found = false;
					for (auto jt = labels.begin(); jt != labels.end(); jt++) {
						if (it->first == (*jt)->mID) {
							found = true;
							break;
						};
					};

					// add a label if not displayed
					if (!found) {
						// create new label
						Edgetime* time = new Edgetime(mLevel, it->first);
						add(time);
						labels.push_back(time);
					};
				};

				// delete inactive labels
				auto it = labels.begin();
				while (it != labels.end()) {
					if ((*it)->active)
						it++;
					else
						it = labels.erase(it);
				};
			});
		};
	};
};