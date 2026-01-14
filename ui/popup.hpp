#pragma once

/// User interface namespace.
namespace UI {
	class NotifManager;

	/// Notification element.
	class Notif : public Element {
		friend NotifManager;

		protected:
		/// Display time left.
		float left;
		/// Text object.
		Text* text;

		/// Initialized notification element.
		void init() {
			box.size = { 1ps, 56px };
			left = 2.5f;

			// create notification text
			text = new Text;
			text->font = 3;
			text->align = Right;
			text->box.pos = { 1ps - 1ws, 0.5ps - 0.5ws + Dim::pix(text->font) };
			add(text);
		};

		public:
		/// Empty update function.
		static const std::function<void(Text&)> None;

		/// Repeatedly updated notification constructor.
		/// @param renew Text update function.
		static Notif* Repeat(std::function<void(Text&)> renew) {
			Notif* notif = new Notif;
			notif->init();
			notif->text->updateCall([=](Element&, float) {
				renew(*notif->text);
			});
			return notif;
		};

		/// Single update notification constructor.
		/// @param renew Text update function.
		static Notif* Once(std::function<void(Text&)> renew) {
			Notif* notif = new Notif;
			notif->init();
			renew(*notif->text);
			return notif;
		};

		/// Returns notification text element.
		Text& data() { return *text; };
	};
	const std::function<void(Text&)> Notif::None = [](Text&){};

	/// Notification manager element.
	class NotifManager : public Element {
		protected:
		/// Notification list.
		std::list<Notif*> items;
		/// Animator list pointer.
		std::list<std::unique_ptr<Animator>>* anims;

		public:
		/// Manager constructor.
		NotifManager (std::list<std::unique_ptr<Animator>>* anims): anims(anims) {
			box = { 16px, 8px, 1ps - 32px, 1ps - 8px };

			// add notification updater
			updateCall([&, anims](Element&, float delta) {
				// update notification alpha
				for (auto it = items.begin(); it != items.end(); it++) {
					// update notification state
					(*it)->left -= delta;
					if ((*it)->left <= 0.0f) {
						(*it)->left = 2.0f;

						// create notification leave animation
						AnimInt* anim = new AnimInt;
						anim->setEasing(Easings::sineIn);
						anim->start(
							&(*it)->box.pos.x.off,
							(*it)->text->box.size.x.off + 16,
							1.0f
						);
						anim->finish([&, it](AnimInt&) {
							remove(*it);
							items.remove(*it);
						});
						anims->push_back(std::unique_ptr<Animator>(anim));
					};
				};
			});
		};

		/// Pushes a notification.
		void push(Notif* notif) {
			// set notification position
			notif->box.pos = { 0px, 1ps };

			// register notification
			items.push_front(notif);
			add(notif);

			// move all notification upwards
			int pos = 0;
			for (Notif* pop : items) {
				AnimInt* anim = new AnimInt;
				anim->setEasing(Easings::sineOut);
				anim->start(&pop->box.pos.y.off, pos -= 56, 0.5f);
				anims->push_back(std::unique_ptr<Animator>(anim));
			};
		};

		/// Pops all notifications.
		void pop() {
			for (Notif* pop : items)
				pop->left = 0.0f;
		};
	};
};