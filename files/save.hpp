#pragma once

/// Data storage namespace.
namespace Data {
	/// Level save data.
	struct LevelData {
		uint16_t      id = 0;       /// Level ID.
		uint16_t      no = 0;       /// Level number.
		std::string name = "";      /// Level name.
		Time        time = {};      /// Level completion time.
		Rank        rank = Rank::N; /// Level completion rank.
		uint16_t    pcol = 0;       /// Collected prism count.
		uint16_t    pttl = 0;       /// Total prism count.
		uint16_t  deaths = 0;       /// Death count.

		/// Generates a level data object.
		/// @param id Level ID.
		/// @param name Level name.
		/// @param pcol Collected prism count.
		/// @param pttl Total prism count.
		/// @param deaths Death count.
		static LevelData generate(uint16_t id, std::string name, uint16_t pcol, uint16_t pttl, uint16_t deaths) {
			return LevelData { id, 0, name, Time(), Rank::N, pcol, pttl, deaths };
		};

		/// Assigns time data to level info.
		/// @param time Completion time.
		/// @param thr Level time thresholds.
		LevelData& setTime(const Time& time, const Thresholds& thr) {
			this->time = time;
			this->rank = thr.rate(time.total(), pcol == pttl);
			return *this;
		};

		/// Assigns a level number to level info.
		/// @param no Level number.
		LevelData& setNo(uint16_t no) {
			this->no = no;
			return *this;
		};
	};
};