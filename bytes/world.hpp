#pragma once

/// Data storage namespace.
namespace Data {
	/// Level record object.
	struct Record {
		uint16_t     id; // Level ID.
		uint16_t prisms; // Collected prisms.
		uint16_t deaths; // Death count.
		Time       time; // Completion time.

		/// Reads record data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Record& read(FileReader& reader) {
			id = reader.u16();
			prisms = reader.u16();
			deaths = reader.u16();

			time.game = reader.u32();
			time.edge = reader.u32();

			Log::out.symI(); Log::out.format(
				"ID = %04X | Time = %d\"%02d'%03d | Prisms = %u, Deaths = %u\n", id,
				Time::min(time.game), Time::sec(time.game), Time::ms(time.game),
				Time::min(time.edge), Time::sec(time.edge), Time::ms(time.edge),
				prisms, deaths
			);
			Log::out.print();
			return *this;
		};

		/// Writes record data.
		/// @param writer File writer object.
		void write(FileWriter& writer) const {
			writer.u16(id);
			writer.u16(prisms);
			writer.u16(deaths);
			writer.u32(time.game);
			writer.u32(time.edge);
		};

		/// Checks if a new record is better than the current one.
		/// @param oth New record.
		bool better(const Record& oth) const {
			// compare prism count
			if (oth.prisms > prisms) return true;
			else if (oth.prisms == prisms) {
				// compare timer
				if (oth.time.total() < time.total()) return true;
				else if (oth.time.total() == time.total()) {
					// compare deaths
					return oth.deaths > deaths;
				};
			};
			return false;
		};

		/// Checks if record is empty.
		bool empty() const { return id == 0; };
	};

	/// Record type enumeration.
	enum RecordType {
		Old, // Record was not overwritten.
		New, // Record was overwritten.
		Add, // Record was added.
	};

	/// World save data.
	struct Save {
		/// Level records.
		std::vector<Record> recs;
		/// Invalid record fuse.
		Record fuse = { 0, 0, 0, {0, 0} };

		/// Reads save data.
		/// @param reader File reader object.
		/// @return Self-reference.
		Save& read(FileReader& reader) {
			Log::out.separator("Save Data");
			Log::out.print();

			// read record count
			uint32_t count = reader.u16();
			Log::out.symI(); Log::out.format("Record count = %u\n", count);
			Log::out.print();

			// read records
			for (uint32_t i = 0; i < count; i++)
				store(Record().read(reader));
			return *this;
		};

		/// Saves save data.
		/// @param writer File writer object.
		void write(FileWriter& writer) const {
			// write record count
			writer.u16(recs.size());

			// write records
			for (const Record& rec : recs)
				rec.write(writer);
		};

		/// Returns a level record.
		/// @param idx Level index.
		/// @note If a level does not have records, an empty record will be returned.
		const Record& get(uint16_t idx) const {
			// try to find a record
			for (size_t i = 0; i < recs.size(); i++) {
				if (recs[i].id == idx)
					return recs[i];
			};

			// return an empty record
			return fuse;
		};

		/// Stores a new level record.
		/// @param record Record object.
		/// @return Record type.
		RecordType store(Record record) {
			// try to find an existing record
			for (size_t i = 0; i < recs.size(); i++) {
				if (recs[i].id == record.id) {
					// check if new record is better
					if (recs[i].better(record)) {
						recs[i] = record;
						return New;
					};
					return Old;
				};
			};

			// push a new record
			recs.push_back(record);
			return Add;
		};
	};

	/// Level collection object.
	struct World {
		/// World map route data.
		Bytes::Routes routedata;
		/// Default level for error output.
		Bytes::Level fallback;

		/// World save data.
		Save save;
		/// Level signature table.
		std::map<std::string, Bytes::Sign> signs;

		/// Level collection.
		std::map<std::string, Bytes::Level> levels;
		/// Permanent level files.
		std::vector<std::string> perms;
		/// Last error type.
		int error = 0;
		/// Whether the last level load was bad.
		bool bad = false;

		/// Default constructor.
		/// @param routes World map route data.
		World (const Bytes::Routes& routes): routedata(routes) {};

		/// Finds a level signature by its filename.
		/// @param file Level file.
		/// @return If no signature was found, an empty one (ID = 0) will be returned.
		Bytes::Sign findFile(const std::string& file) const {
			auto it = signs.find(file);
			if (it == signs.cend())
				return Bytes::Sign { 0, "", {{0, 0, 0, 0}} };
			return it->second;
		};

		/// Finds a level signature by its ID.
		/// @param idx Level ID.
		/// @return If no signature was found, an empty one (ID = 0) will be returned.
		Bytes::Sign findID(uint16_t idx) const {
			for (auto it = signs.cbegin(); it != signs.cend(); it++) {
				if (it->second.ID == idx)
					return it->second;
			};
			return Bytes::Sign { 0, "", {{0, 0, 0, 0}} };
		};

		/// Creates a full level data object from a level record.
		/// @param sign Level signature.
		/// @param record Level record.
		/// @param no Level number.
		LevelData merge(const Bytes::Sign& sign, const Record& record, uint16_t no) const {
			LevelData data = LevelData::generate(sign.ID, sign.name, record.prisms, sign.prisms, record.deaths);
			if (record.id != 0)
				data.setTime(record.time, sign.thresholds);
			return data.setNo(no);
		};

		/// Stores a level record.
		/// @param data Level record data.
		/// @return Record type.
		RecordType store(const LevelData& data) {
			return save.store({ data.id, data.pcol, data.deaths, data.time });
		};

		/// Generates a list of completed level numbers.
		std::vector<size_t> levelNos() {
			std::vector<size_t> result;
			for (size_t i = 0; i < routedata.routes.size(); i++) {
				if (save.get(findFile(routedata.routes[i].file).ID).id != 0)
					result.push_back(i);
			};
			return result;
		};

		/// Returns world stats.
		/// @param[out] pcol Collected prism count.
		/// @param[out] pttl Total prism count.
		/// @return Average rank string.
		const char* stats(uint32_t& pcol, uint32_t& pttl) const {
			// reset stats
			uint32_t rankAcc = 0;
			pcol = 0;
			pttl = 0;

			Log::out.separator("Stats Calculation");
			Log::out.symI(); Log::out.format("Level count = %u\n", (uint32_t)signs.size());
			Log::out.print();

			// process each level
			for (const Bytes::Route& route : routedata.routes) {
				// load route signature
				const Bytes::Sign& sign = findFile(route.file);
				if (sign.ID == 0) continue;

				Log::out.symI(); Log::out.format("ID = %u | Name = %s | ", sign.ID, sign.name.c_str());
				Log::out.print();

				// get level record
				Record record = save.get(sign.ID);

				Log::out.format("Prisms = %u/%u | ", record.prisms, sign.prisms);
				Log::out.format("Time = %d\"%02d'%03d | ",
					Time::min(record.time.total()), Time::sec(record.time.total()), Time::ms(record.time.total()));

				// get level rank
				Rank cur = record.id == sign.ID ? sign.thresholds.rate(
					record.time.total(),
					record.prisms >= sign.prisms
				) : Rank::D;
				Log::out.format("Rank = %s\n", rawRankRepr(cur));
				Log::out.print();

				// update stats
				rankAcc += Rank::D - cur;
				pcol += record.prisms;
				pttl += sign.prisms;
			};

			// calculate average rank
			return avgRankRepr(rankAcc, routedata.routes.size());
		};

		/// Preloads world levels signatures.
		/// @return List of levels that could not be loaded.
		std::vector<std::pair<std::string, int>> preload() {
			signs.clear();
			std::vector<std::pair<std::string, int>> errors;

			Log::out.separator("Preloading");
			Log::out.symI(); Log::out.format("Route count = %u\n", (uint32_t)routedata.routes.size());
			Log::out.print();

			// preload each file in route data list
			for (const Bytes::Route& route : routedata.routes) {
				Log::out.symI();
				Log::out.format("Preloading level <%s>...\n", route.file.c_str());
				Log::out.print();

				// check if level is already loaded
				auto it = levels.find(route.file);
				if (it != levels.end()) {
					signs[route.file] = Bytes::Sign {
						it->second.ID, it->second.name, it->second.thresholds, it->second.prismCount
					};
					Log::out.symI();
					Log::out.format("ID = %04X | Name = %s | Prisms = %u\n",
						it->second.ID, it->second.name.c_str(), it->second.prismCount);
					Log::out.print();
					continue;
				};

				// try to read level file
				int status = 0;
				FileReader file = FileReader::open(("levels/" + route.file + ".dat").c_str(), &status);
				if (status) {
					Log::out.symE();
					Log::out.format("Failed to load level.\n");
					Log::out.print();
					errors.push_back(std::pair(route.file, status));
					continue;
				};

				// store level signature
				signs[route.file] = Bytes::Sign().read(file);
				Log::out.symI();
				Log::out.format("ID = %04X | Name = %s | Prisms = %u\n",
					signs[route.file].ID, signs[route.file].name.c_str(), signs[route.file].prisms);
				Log::out.print();
			};
			return errors;
		};

		/// Adds a file to collection.
		/// @param path Level file path.
		/// @param file File reader object.
		/// @return Level object pointer.
		const Bytes::Level& add(const std::string& path, FileReader&& file) {
			try {
				levels[path] = Bytes::Level().read(file);
				bad = false;
			} catch (EOFError err) {
				bad = true;
				return fallback;
			};
			perms.push_back(path);
			return levels[path];
		};

		/// Returns a level object.
		/// @param path Level file path.
		/// @return Level object pointer.
		const Bytes::Level& load(const std::string& path) {
			// check if level is present in collection
			auto idx = levels.find(path);
			if (idx != levels.end())
				return idx->second;

			// try to read level file
			int status = 0;
			FileReader file = FileReader::open(("levels/" + path + ".dat").c_str(), &status);
			if (status) {
				error = status;
				return fallback;
			};
			error = 0;

			// register level
			return add(path, std::move(file));
		};

		/// Unloads a level from collection.
		/// @param path Level file path.
		void unload(const std::string& path) {
			// ignore if level is loaded permanently
			if (std::find(perms.cbegin(), perms.cend(), path) != perms.cend())
				return;

			// erase level data
			auto idx = levels.find(path);
			if (idx != levels.end())
				levels.erase(idx);
		};
	};
};