#pragma once

/// File writer object.
class FileWriter {
	protected:
	FILE* m_desc; // File descriptor.
	char* m_name; // File name.

	public:
	/// Empty constructor.
	FileWriter (): m_desc(NULL), m_name(NULL) {};
	/// Deleted copy constructor.
	FileWriter (const FileWriter& writer) = delete;
	/// Deleted copy assignment.
	FileWriter& operator=(const FileWriter& writer) = delete;
	/// Move constructor.
	/// @param writer Moved file writer object.
	FileWriter (FileWriter&& writer) {
		writer.m_desc = NULL;
		writer.m_name = NULL;
	};
	/// Move assignment.
	/// @param writer Moved file writer object.
	FileWriter& operator=(FileWriter&& writer) {
		m_desc = writer.m_desc;
		m_name = writer.m_name;
		writer.m_desc = NULL;
		writer.m_name = NULL;
		return *this;
	};
	/// File writer destructor.
	~FileWriter () { if (m_desc) fclose(m_desc); m_desc = NULL; delete[] m_name; };

	/// Opens a file to write.
	/// @param filepath Written file path.
	/// @param status File status output.
	/// @return File writer object.
	static FileWriter open(const char* filepath, int* status = NULL) {
		FileWriter writer;

		// copy file name
		writer.m_name = getfilename(filepath);

		// open file stream
		writer.m_desc = fopen(filepath, "wb");
		if (writer.m_desc == NULL)
			if (status) *status = errno;
		return writer;
	};

	/// Returns file name.
	const char* name() const { return m_name; };

	/// Writes an unsigned 8-bit integer.
	void u8(uint8_t val) { fputc(val, m_desc); };
	/// Writes an unsigned 16-bit integer.
	void u16(uint16_t val) { u8(val); u8(val >> 8); };
	/// Writes an unsigned 32-bit integer.
	void u32(uint32_t val) { u16(val); u16(val >> 16); };
	/// Writes an unsigned 64-bit integer.
	void u64(uint64_t val) { u32(val); u32(val >> 32); };

	/// Writes a signed 8-bit integer.
	void s8(int8_t val) { u8(val); };
	/// Writes a signed 16-bit integer.
	void s16(int8_t val) { u16(val); };
	/// Writes a signed 32-bit integer.
	void s32(int8_t val) { u32(val); };
	/// Writes a signed 64-bit integer.
	void s64(int8_t val) { u64(val); };

	/// Writes a specified amount of bytes.
	/// @param buffer Byte buffer.
	/// @param count Byte count.
	void str(const uint8_t* buffer, size_t count) {
		for (size_t i = 0; i < count; i++)
			u8(buffer[i]);
	};
};