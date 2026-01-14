#pragma once

/// User interface namespace.
namespace UI {
	/// Blind element.
	class Blind : public Panel {
		public:
		/// Blind constructor.
		Blind () {
			// setup blind panel
			box = { 0px, 0px, 1ps, 1ps };
			color = glm::vec3(0.0f);
			alpha = 0.0f;
		};
	};

	/// Level start info wall element.
	class Wall : public Element {
		public:
		/// Info blind.
		Blind* blind;

		/// Empty wall constructor.
		Wall (): blind(NULL) {};

		/// Wall constructor.
		/// @param sign Level signature.
		/// @param record Level record.
		/// @param no Level number.
		Wall (Bytes::Sign sign, Data::Record record, uint16_t no) {
			// setup base
			box = { 0px, 0px, 1ps, 1ps };
			{
				// create upper panel
				Panel* upper = new Panel;
				upper->box = { 0px, 0px, 1ps, 68px };
				upper->color = RGB(0x3B3B3B);
				{
					// create level number container
					Element* cont = new Element;
					cont->box = { 0px, 0px, 96px, 64px };
					upper->add(cont);

					// create level number
					Text* num = new Text;
					num->font = 5;
					num->box.pos = { 0.5ps - 0.5ws, 0.5ps - 0.5ws + Dim::pix(num->font) };
					num->setFmt("%u", no);
					cont->add(num);
				};
				{
					// create previous record panel
					Panel* rPanel = new Panel;
					rPanel->box = { 96px, 0px, 1ps - 96px, 64px };
					rPanel->color = RGB(0x191B1F);
					upper->add(rPanel);

					// create record label
					if (record.id != 0) {
						Text* rText = new Text;
						rText->font = 4;
						rText->box.pos = { 1ps - 1ws - 8px, 0.5ps - 0.5ws + Dim::pix(rText->font) };
						rText->setFmt(
							"Record: \x19\6%d>%02d'%03d",
							Data::Time::min(record.time.total()),
							Data::Time::sec(record.time.total()),
							Data::Time::ms(record.time.total())
						);
						rPanel->add(rText);
					};
				};
				add(upper);
			};
			{
				// create level info
				Text* text = new Text;
				text->font = 4;
				text->align = Center;
				text->box.pos = { 0.5ps - 0.5ws, 0.5ps - 0.5ws };
				text->setFmt(
					"%s\n"
					"\n\x19\6%s\7 - %d>%02d'%03d"
					"\n\x19\6%s\7 - %d>%02d'%03d"
					"\n\x19\6%s\7 - %d>%02d'%03d"
					"\n\x19\6%s\7 - %d>%02d'%03d",
					sign.name.c_str(),
					Data::rawRankRepr(Data::S),
						Data::Time::min(sign.thresholds.time[Data::S] * 1000),
						Data::Time::sec(sign.thresholds.time[Data::S] * 1000),
						Data::Time::ms (sign.thresholds.time[Data::S] * 1000),
					Data::rawRankRepr(Data::A),
						Data::Time::min(sign.thresholds.time[Data::A] * 1000),
						Data::Time::sec(sign.thresholds.time[Data::A] * 1000),
						Data::Time::ms (sign.thresholds.time[Data::A] * 1000),
					Data::rawRankRepr(Data::B),
						Data::Time::min(sign.thresholds.time[Data::B] * 1000),
						Data::Time::sec(sign.thresholds.time[Data::B] * 1000),
						Data::Time::ms (sign.thresholds.time[Data::B] * 1000),
					Data::rawRankRepr(Data::C),
						Data::Time::min(sign.thresholds.time[Data::C] * 1000),
						Data::Time::sec(sign.thresholds.time[Data::C] * 1000),
						Data::Time::ms (sign.thresholds.time[Data::C] * 1000)
				);
				add(text);
			};
			{
				// create wall blind
				blind = new Blind();
				blind->alpha = 1.0f;
				add(blind);
			};
		};
	};
};