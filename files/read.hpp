#pragma once

/// Read after end of file error.
struct EOFError {
	std::string name;
};

/// File reader object.
class FileReader {
	protected:
	uint8_t* m_data; // File data.
	size_t   m_size; // File size.
	size_t   m_ptr ; // File pointer.
	char*    m_name; // File name.

	public:
	/// Empty constructor.
	FileReader (): m_data(NULL), m_size(0), m_ptr(0), m_name(NULL) {};
	/// Deleted copy constructor.
	FileReader (const FileReader& reader) = delete;
	/// Deleted copy assignment.
	FileReader& operator=(const FileReader& reader) = delete;
	/// Move constructor.
	/// @param reader Moved file reader object.
	FileReader (FileReader&& reader): m_data(reader.m_data), m_size(reader.m_size), m_ptr(reader.m_ptr), m_name(reader.m_name) {
		reader.m_data = NULL;
		reader.m_name = NULL;
	};
	/// Move assignment.
	/// @param reader Moved file reader object.
	/// @return Self-reference.
	FileReader& operator=(FileReader&& reader) {
		m_data = reader.m_data;
		m_size = reader.m_size;
		m_ptr  = reader.m_ptr ;
		m_name = reader.m_name;
		reader.m_data = NULL;
		reader.m_name = NULL;
		return *this;
	};
	/// File reader destructor.
	~FileReader () { delete[] m_data; delete[] m_name; };

	/// Reads a file.
	/// @param filepath Read file path.
	/// @param status File read status output.
	/// @return File reader object.
	static FileReader open(const char* filepath, int* status = NULL) {
		FileReader reader;

		// copy file name
		reader.m_name = getfilename(filepath);

		// open file stream
		FILE* fp = fopen(filepath, "rb");
		if (fp == NULL) {
			if (status) *status = errno;
			return reader;
		};

		// read characters
		std::vector<uint8_t> data;
		while (true) {
			char c = fgetc(fp);
			if (feof(fp)) break;
			data.push_back(c);
		};
		fclose(fp);

		// store data in a reader object
		reader.m_data = new uint8_t[data.size()];
		reader.m_size = data.size();
		memcpy(reader.m_data, data.data(), data.size());
		return reader;
	};

	/// Creates a file reader object from static data.
	/// @param path File path (only for display).
	/// @param data File data.
	/// @param size File size.
	/// @return File reader object.
	static FileReader text(const char* path, const uint8_t* data, size_t size) {
		FileReader reader;

		// copy file name
		reader.m_name = getfilename(path);

		// copy file data
		reader.m_data = new uint8_t[size];
		reader.m_size = size;
		memcpy(reader.m_data, data, size);
		return reader;
	};

	/// Returns file name.
	const char* name() const { return m_name; };

	/// Returns file size.
	size_t size() const { return m_size; };
	/// Checks whether there is any more data to read.
	bool left() const { return m_ptr < m_size; };
	/// Checks if specified amount of bytes are available.
	bool available(size_t count) const { return m_ptr + count <= m_size; };
	/// Checks if the whole file has been read properly.
	bool end() { return m_ptr == m_size; };

	/// Returns to the start of file.
	void rewind() { m_ptr = 0; };

	/// Reads an unsigned 8-bit integer.
	uint8_t u8() { if (m_ptr >= m_size) throw EOFError { m_name }; return m_data[m_ptr++]; };

	/// Reads an unsigned 16-bit integer.
	uint16_t u16() { return (uint16_t)u8 () | (uint16_t)u8 () << 8 ; };
	/// Reads an unsigned 32-bit integer.
	uint32_t u32() { return (uint32_t)u16() | (uint32_t)u16() << 16; };
	/// Reads an unsigned 64-bit integer.
	uint64_t u64() { return (uint64_t)u32() | (uint64_t)u32() << 32; };

	/// Reads a signed 8-bit integer.
	int8_t  s8 () { return (int8_t) u8 (); };
	/// Reads a signed 16-bit integer.
	int16_t s16() { return (int16_t)u16(); };
	/// Reads a signed 32-bit integer.
	int32_t s32() { return (int32_t)u32(); };
	/// Reads a signed 64-bit integer.
	int64_t s64() { return (int64_t)u64(); };

	/// Reads a specified amount of bytes.
	/// @param buffer Output buffer.
	/// @param count Amount of bytes to read.
	/// @return Amount of bytes read.
	size_t str(uint8_t* buffer, size_t count) {
		for (size_t i = 0; i < count; i++) {
			if (!left()) return i;
			buffer[i] = u8();
		};
		return count;
	};
};

/// Bit reader helper object.
class BitReader {
	private:
		FileReader* reader; // Bit source.
		uint8_t queue;      // Bit queue.
		uint8_t left;       // Bits left.

	public:
		/// Empty constructor.
		BitReader (FileReader* reader): reader(reader), queue(0), left(0) {};

		/// Reads a specified amount of bits.
		/// @param count Amount of bits.
		/// @return Read bit array.
		uint64_t read(uint8_t count) {
			uint64_t array = 0;

			// checks for bits inside a byte
			if (count < left) {
				array = queue >> (left -= count);
				queue &= (1 << left) - 1;
				return array;
			};

			// read until byte end
			array = queue;
			count -= left;
			queue = 0;
			left = 0;

			// read byte aligned bits
			while (count >= 8) {
				array <<= 8;
				array |= reader->u8();
				count -= 8;
			};

			// read last bits
			if (count != 0) {
				left = 8 - count;
				queue = reader->u8();
				array <<= count;
				array |= queue >> left;
				queue &= (1 << left) - 1;
			};
			return array;
		};
};