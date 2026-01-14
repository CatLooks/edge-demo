#pragma once

/// Music namespace.
namespace Music {
	/// Music enumeration.
	enum Music {
		NotSoCubic,
		Eternity,
		Quiet,
		Pad,
		Tec,
		Kakkoi,
		Dark,
		Squadron,
		EightBits,
		Pixel,
		Jupiter,
		Shame,
		VoyageGeometrique,
		MZone,
		R2,
		MysteryCube,
		Duty,
		Fun,
		Lol,
		LostWay,
		WallStreet,
		Debrief,
		Space
	};

	/// Returns music track file name.
	/// @param theme Music track.
	const char* getFile(Music theme) {
		const char* table[] {
			"music/title.ogg",
			"music/eternity.ogg",
			"music/quiet.ogg",
			"music/pad.ogg",
			"music/tec.ogg",
			"music/kakkoi.ogg",
			"music/dark.ogg",
			"music/squadron.ogg",
			"music/8bits.ogg",
			"music/pixel.ogg",
			"music/jupiter.ogg",
			"music/shame.ogg",
			"music/voyage.ogg",
			"music/mzone.ogg",
			"music/r2.ogg",
			"music/mystery.ogg",
			"music/duty.ogg",
			"music/fun.ogg",
			"music/lol.ogg",
			"music/lostway.ogg",
			"music/wallstreet.ogg",
			"music/debrief.ogg",
			"music/space.ogg"
		};
		return table[(theme >= 0 && theme < Space) ? theme : Space];
	};

	/// Music player.
	sf::Music bgm;
	/// Whether the music is fading.
	bool fading = false;

	/// Starts to play a music track.
	/// @param theme Music track.
	/// @return Whether the file was opened properly.
	bool start(Music theme) {
		// load music file
		if (!bgm.openFromFile(getFile(theme)))
			return false;

		// start playing
		bgm.setLoop(true);
		bgm.setVolume(100.0f);
		bgm.play();
		fading = false;
		return true;
	};

	/// Updates music volume.
	/// @param delta Time elapsed since last update.
	void update(float delta) {
		// ignore if not fading
		if (!fading) return;

		// get music volume
		float t = bgm.getVolume() / 100.0f;
		t = glm::clamp(t - delta * 2.0f, 0.0f, 1.0f);

		// set music volume
		bgm.setVolume(t * 100.0f);
		if (t <= 0.0f) fading = false;
	};

	/// Starts to fade out music.
	void fade() { fading = true; };

	/// Stops music.
	void stop() { bgm.stop(); };
};