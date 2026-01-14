#pragma once

/// User interface namespace.
namespace UI {
	/// Level label object.
	class LevelInfo : public Text {
		public:
		/// Display camera.
		Camera* camera;
		/// Displayed info.
		Data::LevelData data;
		/// Label position.
		vec3x pos;

		/// Label text constructor.
		/// @param camera Level camera.
		LevelInfo (Camera* camera): camera(camera) {
			box.pos = { -0.5ws, -1ws };
			font = 3;
			align = Center;
			if (camera) updateCall([=](Element&, float) {
				// update label position
				sf::IntRect rect = camera->vertBounds(vol3x(pos, vec3x(1)));
				sf::Vector2i vec = { rect.left + rect.width / 2, rect.top };
				box.pos.x.off = vec.x;
				box.pos.y.off = vec.y;
			});
		};

		/// Emits fade out animation.
		/// @param anims Animator list.
		void emitHide(std::list<std::unique_ptr<Animator>>& anims) {
			AnimFloat* anim = new AnimFloat;
			anim->setEasing(Easings::sineIn);
			anim->start(&box.pos.y.box, -1.0f, -0.3f, 0.2f);
			anim->finish([&](AnimFloat&) { active = false; });
			anims.push_back(std::unique_ptr<Animator>(anim));
		};

		/// Emits fade in animation.
		/// @param anims Animator list.
		void emitShow(std::list<std::unique_ptr<Animator>>& anims) {
			AnimFloat* anim = new AnimFloat;
			anim->setEasing(Easings::sineOut);
			anim->start(&box.pos.y.box, -0.3f, -1.0f, 0.2f);
			anims.push_back(std::unique_ptr<Animator>(anim));
			active = true;
		};

		/// Displays new level stats.
		void display() {
			// generate prism list
			char prism[32];
			genPrismList(prism, data.pttl, data.pttl - data.pcol, true);

			// update label
			active = true;
			setFmt(
				"\x19\2%s\n\6%s\x12\x18\n\1\2Rank: %s \4|\1\2 Time: %d>%02d'%03d",
				data.name.c_str(), prism, Data::rankRepr(data.rank),
				Data::Time::min(data.time.total()),
				Data::Time::sec(data.time.total()),
				Data::Time::ms(data.time.total())
			);
		};
	};
};