#ifndef SVG_PATH_TOKENIZER_H_
#define SVG_PATH_TOKENIZER_H_

class path_tokenizer
{
 public:
	path_tokenizer();

	void set_path_str(const char* str);
	bool next();

	double next(char cmd);

	char   last_command() const { return m_last_command; }
	double last_number() const { return m_last_number; }

private:
	static void init_char_mask(char* mask, const char* char_set);

	bool contains(const char* mask, unsigned c) const
	{
		return (mask[(c >> 3) & (256/8-1)] & (1 << (c & 7))) != 0;
	}

	bool is_command(unsigned c) const
	{
		return contains(m_commands_mask, c);
	}

	bool is_numeric(unsigned c) const
	{
		return contains(m_numeric_mask, c);
	}

	bool is_separator(unsigned c) const
	{
		return contains(m_separators_mask, c);
	}

	bool parse_number();

	char m_separators_mask[256/8];
	char m_commands_mask[256/8];
	char m_numeric_mask[256/8];

	const char* m_path;
	double m_last_number;
	char   m_last_command;

	static const char s_commands[];
	static const char s_numeric[];
	static const char s_separators[];

};

#endif // SVG_PATH_TOKENIZER_
