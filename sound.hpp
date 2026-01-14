#pragma once

/// Assets namespace.
namespace Assets {
	/// Sound effect namespace.
	namespace Sound {
		std::deque<sf::SoundBuffer> buffers;

		// gameplay sounds
		sf::Sound cube[2][5];
		sf::Sound fall;
		sf::Sound f_decay;
		sf::Sound f_drop;
		sf::Sound moving;
		sf::Sound bump;
		sf::Sound resize[2];
		sf::Sound button;
		sf::Sound prism;
		sf::Sound exit;

		// menu sounds
		sf::Sound m_select;
		sf::Sound m_cancel;
		sf::Sound m_ok;
		sf::Sound m_start;

		// level name sound
		sf::SoundBuffer levelBuffer;
		sf::Sound levelSound;
	};

	/// Plays level title sound effect.
	/// @param id Level ID.
	void nameSfx(uint16_t id) {
		// load target file
		char* file = formatf("sound/levels/%04X.wav", id);
		if (!Sound::levelBuffer.loadFromFile(file)) return;
		delete file;

		// play sound
		Sound::levelSound.stop();
		Sound::levelSound.setBuffer(Sound::levelBuffer);
		Sound::levelSound.play();
	};

	/// Checks whether level title sound effect has stopped playing.
	bool nameSfxDone() { return Sound::levelSound.getPlayingOffset() == sf::Time::Zero; };

	/// Sound effect enumeration.
	enum SoundEffect {
		Cube, Minicube, Fall, Fallstop,
		Decay, Drop, Tick, Bump,
		Shrink, Grow, Button, Prism, Dark,
		Exit, Select, Cancel, Ok, Start
	};

	/// Plays a sound effect.
	/// @param effect Sound effect ID.
	void sfx(SoundEffect effect) {
		switch (effect) {
			case Cube    : Sound::cube[0][rand() % 5].play(); break;
			case Minicube: Sound::cube[1][rand() % 5].play(); break;
			case Fall    : Sound::fall.play();                break;
			case Fallstop: Sound::fall.stop();                break;
			case Decay   : Sound::f_decay.play();             break;
			case Drop    : Sound::f_drop.play();              break;
			case Tick    : Sound::moving.play();              break;
			case Bump    : Sound::bump.play();                break;
			case Shrink  : Sound::resize[0].play();           break;
			case Grow    : Sound::resize[1].play();           break;
			case Button  : Sound::button.play();              break;
			case Exit    : Sound::exit.play();                break;
			case Select  : Sound::m_select.play();            break;
			case Cancel  : Sound::m_cancel.play();            break;
			case Ok      : Sound::m_ok.play();                break;
			case Start   : Sound::m_start.play();             break;
			case Prism:
				Sound::prism.setPitch(1.0f);
				Sound::prism.play();
				break;
			case Dark:
				Sound::prism.setPitch(powf(2.0f, -1.25f / 12.0f));
				Sound::prism.play();
				break;
			default: break;
		};
	};

	/// Pause all long game sounds.
	void pauseSfx() {
		Sound::fall.pause();
		Sound::f_drop.pause();
		Sound::resize[0].pause();
		Sound::resize[1].pause();
		Sound::exit.pause();
	};

	/// Resume all long game sounds.
	void resumeSfx() {
		if (Sound::fall     .getPlayingOffset() != sf::Time::Zero) Sound::fall.play();
		if (Sound::f_drop   .getPlayingOffset() != sf::Time::Zero) Sound::f_drop.play();
		if (Sound::resize[0].getPlayingOffset() != sf::Time::Zero) Sound::resize[0].play();
		if (Sound::resize[1].getPlayingOffset() != sf::Time::Zero) Sound::resize[1].play();
		if (Sound::exit     .getPlayingOffset() != sf::Time::Zero) Sound::exit.play();
	};

	/// Loads all sound effect assets.
	void loadSfx() {
		Log::out.separator("Sound effects");

		// sound effect assets import data
		struct sfx_t {
			const char* filename;
			sf::Sound* target;
		};
		const sfx_t sounds[] {
			{ "sound/cube0.wav", &Sound::cube[0][0] },
			{ "sound/cube1.wav", &Sound::cube[0][1] },
			{ "sound/cube2.wav", &Sound::cube[0][2] },
			{ "sound/cube3.wav", &Sound::cube[0][3] },
			{ "sound/cube4.wav", &Sound::cube[0][4] },

			{ "sound/mini0.wav", &Sound::cube[1][0] },
			{ "sound/mini1.wav", &Sound::cube[1][1] },
			{ "sound/mini2.wav", &Sound::cube[1][2] },
			{ "sound/mini3.wav", &Sound::cube[1][3] },
			{ "sound/mini4.wav", &Sound::cube[1][4] },

			{ "sound/fall.wav"  , &Sound::fall      },
			{ "sound/decay.wav" , &Sound::f_decay   },
			{ "sound/drop.wav"  , &Sound::f_drop    },
			{ "sound/move.wav"  , &Sound::moving    },
			{ "sound/bump.wav"  , &Sound::bump      },

			{ "sound/shrink.wav", &Sound::resize[0] },
			{ "sound/grow.wav"  , &Sound::resize[1] },

			{ "sound/press.wav" , &Sound::button    },
			{ "sound/prism.wav" , &Sound::prism     },
			{ "sound/exit.wav"  , &Sound::exit      },

			{ "sound/select.wav", &Sound::m_select  },
			{ "sound/cancel.wav", &Sound::m_cancel  },
			{ "sound/ok.wav"    , &Sound::m_ok      },
			{ "sound/start.wav" , &Sound::m_start   },
		};

		// import each sound effect
		for (const sfx_t& data : sounds) {
			sf::Sound& sfx = *data.target;

			// log data
			Log::out.symI();
			Log::out.format("Loading <%s>.\n", data.filename);
			Log::out.print();

			// load sound to buffer
			sf::SoundBuffer buffer;
			if (!buffer.loadFromFile(data.filename)) {
				Log::out.symE();
				Log::out.queue("Failed to load sound.\n");
				Log::out.print();
				continue;
			};

			// store sound
			Sound::buffers.push_back(buffer);
			sfx.setBuffer(Sound::buffers.back());
		};
	};
};