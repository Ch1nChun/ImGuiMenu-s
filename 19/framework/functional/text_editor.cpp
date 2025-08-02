#include "../settings/functions.h"
#include <algorithm>
#include <chrono>

template<class InputIt1, class InputIt2, class BinaryPredicate>
bool equals(InputIt1 first1, InputIt1 last1,
	InputIt2 first2, InputIt2 last2, BinaryPredicate p)
{
	for (; first1 != last1 && first2 != last2; ++first1, ++first2)
	{
		if (!p(*first1, *first2))
			return false;
	}
	return first1 == last1 && first2 == last2;
}

c_text_editor::c_text_editor()
	: m_line_spacing(1.3f)
	, m_undo_index(0)
	, m_tab_size(4)
	, m_overwrite(false)
	, m_read_only(false)
	, m_within_render(false)
	, m_scroll_to_cursor(false)
	, m_scroll_to_top(false)
	, m_text_changed(false)
	, m_colorizer_enabled(true)
	, m_text_start(20.0f)
	, m_left_margin(10)
	, m_cursor_position_changed(false)
	, m_color_range_min(0)
	, m_color_range_max(0)
	, m_selection_mode(selection_mode::normal)
	, m_check_comments(true)
	, m_last_click(-1.0f)
	, m_handle_keyboard_inputs(true)
	, m_handle_mouse_inputs(true)
	, m_ignore_imgui_child(false)
	, m_show_whitespaces(true)
	, m_start_time(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
{
	m_pallete_base =
	{
		0xff7f7f7f,	// Default
		0xffd69c56,	// Keyword	
		0xff00ff00,	// Number
		0xff7070e0,	// string
		0xff70a0e0, // Char literal
		0xffffffff, // Punctuation
		0xff408080,	// Preprocessor
		0xffaaaaaa, // identifier
		0xff9bc64d, // Known identifier
		0xffc040a0, // Preproc identifier
		0xff206020, // Comment (single line)
		0xff406020, // Comment (multi line)
		0xff101010, // Background
		0xffe0e0e0, // Cursor
		0x10ffffff, // Selection
		0x800020ff, // ErrorMarker
		0x40f08000, // breakpoint
		0xffc5c5c5, // line number
		0x40000000, // Current line fill
		0x40808080, // Current line fill (inactive)
		0x40a0a0a0, // Current line edge
	};

	static bool inited = false;
	static language_definition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"and", "break", "do", "", "else", "elseif", "end", "false", "for", "function", "if", "in", "", "local", "nil", "not", "or", "repeat", "return", "then", "true", "until", "while"
		};

		for (auto& k : keywords)
			langDef.m_keywords.insert(k);

		static const char* const identifiers[] = {
			"assert", "collectgarbage", "dofile", "error", "getmetatable", "ipairs", "loadfile", "load", "loadstring",  "next",  "pairs",  "pcall",  "print",  "rawequal",  "rawlen",  "rawget",  "rawset",
			"select",  "setmetatable",  "tonumber",  "tostring",  "type",  "xpcall",  "_G",  "_VERSION","arshift", "band", "bnot", "bor", "bxor", "btest", "extract", "lrotate", "lshift", "replace",
			"rrotate", "rshift", "create", "resume", "running", "status", "wrap", "yield", "isyieldable", "debug","getuservalue", "gethook", "getinfo", "getlocal", "getregistry", "getmetatable",
			"getupvalue", "upvaluejoin", "upvalueid", "setuservalue", "sethook", "setlocal", "setmetatable", "setupvalue", "traceback", "close", "flush", "input", "lines", "open", "output", "popen",
			"read", "tmpfile", "type", "write", "close", "flush", "lines", "read", "seek", "setvbuf", "write", "__gc", "__tostring", "abs", "acos", "asin", "atan", "ceil", "cos", "deg", "exp", "tointeger",
			"floor", "fmod", "ult", "log", "max", "min", "modf", "rad", "random", "randomseed", "sin", "sqrt", "string", "tan", "type", "atan2", "cosh", "sinh", "tanh",
			"pow", "frexp", "ldexp", "log10", "pi", "huge", "maxinteger", "mininteger", "loadlib", "searchpath", "seeall", "preload", "cpath", "path", "searchers", "loaded", "module", "require", "clock",
			"date", "difftime", "execute", "exit", "getenv", "remove", "rename", "setlocale", "time", "tmpname", "byte", "char", "dump", "find", "format", "gmatch", "gsub", "len", "lower", "match", "rep",
			"reverse", "sub", "upper", "pack", "packsize", "unpack", "concat", "maxn", "insert", "pack", "unpack", "remove", "move", "sort", "offset", "codepoint", "char", "len", "codes", "charpattern",
			"coroutine", "table", "io", "os", "string", "utf8", "bit32", "math", "debug", "package"
		};
		for (auto& k : identifiers)
		{
			identifier id;
			id.m_declaration = "Built-in function";
			langDef.m_identifiers.insert(std::make_pair(std::string(k), id));
		}

		langDef.m_token_regex_strings.push_back(std::make_pair<std::string, pallete_index>("L?\\\"(\\\\.|[^\\\"])*\\\"", pallete_index::string));
		langDef.m_token_regex_strings.push_back(std::make_pair<std::string, pallete_index>("\\\'[^\\\']*\\\'", pallete_index::string));
		langDef.m_token_regex_strings.push_back(std::make_pair<std::string, pallete_index>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", pallete_index::number));
		langDef.m_token_regex_strings.push_back(std::make_pair<std::string, pallete_index>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", pallete_index::number));
		langDef.m_token_regex_strings.push_back(std::make_pair<std::string, pallete_index>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", pallete_index::number));
		langDef.m_token_regex_strings.push_back(std::make_pair<std::string, pallete_index>("[a-zA-Z_][a-zA-Z0-9_]*", pallete_index::identifier));
		langDef.m_token_regex_strings.push_back(std::make_pair<std::string, pallete_index>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", pallete_index::punctuation));

		langDef.m_comment_start = "--[[";
		langDef.m_comment_end = "]]";
		langDef.m_single_line_comment = "--";

		langDef.m_case_sensitive = true;
		langDef.m_auto_indentation = false;

		langDef.m_name = "Lua";

		inited = true;
	}

	m_language_definition = langDef;
	m_regex_list.clear();

	for (auto& r : m_language_definition.m_token_regex_strings)
		m_regex_list.push_back(std::make_pair(std::regex(r.first, std::regex_constants::optimize), r.second));

	colorize();

	m_lines.push_back(line());
}

c_text_editor::~c_text_editor()
{
}

std::string c_text_editor::get_text(const coordinates& aStart, const coordinates& aEnd) const
{
	std::string result;

	auto lstart = aStart.m_line;
	auto lend = aEnd.m_line;
	auto istart = get_character_index(aStart);
	auto iend = get_character_index(aEnd);
	size_t s = 0;

	for (size_t i = lstart; i < lend; i++)
		s += m_lines[i].size();

	result.reserve(s + s / 8);

	while (istart < iend || lstart < lend)
	{
		if (lstart >= (int)m_lines.size())
			break;

		auto& line = m_lines[lstart];
		if (istart < (int)line.size())
		{
			result += line[istart].m_char;
			istart++;
		}
		else
		{
			istart = 0;
			++lstart;
			result += '\n';
		}
	}

	return result;
}

c_text_editor::coordinates c_text_editor::get_actual_cursor_coordinates() const
{
	return sanitize_coordinates(m_state.m_cursor_position);
}

c_text_editor::coordinates c_text_editor::sanitize_coordinates(const coordinates& aValue) const
{
	auto line = aValue.m_line;
	auto column = aValue.m_column;
	if (line >= (int)m_lines.size())
	{
		if (m_lines.empty())
		{
			line = 0;
			column = 0;
		}
		else
		{
			line = (int)m_lines.size() - 1;
			column = get_line_max_column(line);
		}
		return coordinates(line, column);
	}
	else
	{
		column = m_lines.empty() ? 0 : min(column, get_line_max_column(line));
		return coordinates(line, column);
	}
}

// https://en.wikipedia.org/wiki/UTF-8
// We assume that the char is a standalone character (<128) or a leading byte of an UTF-8 code sequence (non-10xxxxxx code)
static int UTF8CharLength(c_text_editor::Char c)
{
	if ((c & 0xFE) == 0xFC)
		return 6;
	if ((c & 0xFC) == 0xF8)
		return 5;
	if ((c & 0xF8) == 0xF0)
		return 4;
	else if ((c & 0xF0) == 0xE0)
		return 3;
	else if ((c & 0xE0) == 0xC0)
		return 2;
	return 1;
}

// "Borrowed" from ImGui source
static inline int ImTextCharToUtf8(char* buf, int buf_size, unsigned int c)
{
	if (c < 0x80)
	{
		buf[0] = (char)c;
		return 1;
	}
	if (c < 0x800)
	{
		if (buf_size < 2) return 0;
		buf[0] = (char)(0xc0 + (c >> 6));
		buf[1] = (char)(0x80 + (c & 0x3f));
		return 2;
	}
	if (c >= 0xdc00 && c < 0xe000)
	{
		return 0;
	}
	if (c >= 0xd800 && c < 0xdc00)
	{
		if (buf_size < 4) return 0;
		buf[0] = (char)(0xf0 + (c >> 18));
		buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
		buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
		buf[3] = (char)(0x80 + ((c) & 0x3f));
		return 4;
	}
	//else if (c < 0x10000)
	{
		if (buf_size < 3) return 0;
		buf[0] = (char)(0xe0 + (c >> 12));
		buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
		buf[2] = (char)(0x80 + ((c) & 0x3f));
		return 3;
	}
}

void c_text_editor::advance(coordinates& aCoordinates) const
{
	if (aCoordinates.m_line < (int)m_lines.size())
	{
		auto& line = m_lines[aCoordinates.m_line];
		auto cindex = get_character_index(aCoordinates);

		if (cindex + 1 < (int)line.size())
		{
			auto delta = UTF8CharLength(line[cindex].m_char);
			cindex = min(cindex + delta, (int)line.size() - 1);
		}
		else
		{
			++aCoordinates.m_line;
			cindex = 0;
		}
		aCoordinates.m_column = get_character_column(aCoordinates.m_line, cindex);
	}
}

void c_text_editor::delete_range(const coordinates& aStart, const coordinates& aEnd)
{
	assert(aEnd >= aStart);
	assert(!m_read_only);

	//printf("D(%d.%d)-(%d.%d)\n", aStart.m_line, aStart.m_column, aEnd.m_line, aEnd.m_column);

	if (aEnd == aStart)
		return;

	auto start = get_character_index(aStart);
	auto end = get_character_index(aEnd);

	if (aStart.m_line == aEnd.m_line)
	{
		auto& line = m_lines[aStart.m_line];
		auto n = get_line_max_column(aStart.m_line);
		if (aEnd.m_column >= n)
			line.erase(line.begin() + start, line.end());
		else
			line.erase(line.begin() + start, line.begin() + end);
	}
	else
	{
		auto& firstLine = m_lines[aStart.m_line];
		auto& lastLine = m_lines[aEnd.m_line];

		firstLine.erase(firstLine.begin() + start, firstLine.end());
		lastLine.erase(lastLine.begin(), lastLine.begin() + end);

		if (aStart.m_line < aEnd.m_line)
			firstLine.insert(firstLine.end(), lastLine.begin(), lastLine.end());

		if (aStart.m_line < aEnd.m_line)
			remove_line(aStart.m_line + 1, aEnd.m_line + 1);
	}

	m_text_changed = true;
}

int c_text_editor::insert_text_at(coordinates& /* inout */ aWhere, const char* aValue)
{
	assert(!m_read_only);

	int cindex = get_character_index(aWhere);
	int totalLines = 0;
	while (*aValue != '\0')
	{
		assert(!m_lines.empty());

		if (*aValue == '\r')
		{
			// skip
			++aValue;
		}
		else if (*aValue == '\n')
		{
			if (cindex < (int)m_lines[aWhere.m_line].size())
			{
				auto& newLine = insert_line(aWhere.m_line + 1);
				auto& line = m_lines[aWhere.m_line];
				newLine.insert(newLine.begin(), line.begin() + cindex, line.end());
				line.erase(line.begin() + cindex, line.end());
			}
			else
			{
				insert_line(aWhere.m_line + 1);
			}
			++aWhere.m_line;
			aWhere.m_column = 0;
			cindex = 0;
			++totalLines;
			++aValue;
		}
		else
		{
			auto& line = m_lines[aWhere.m_line];
			auto d = UTF8CharLength(*aValue);
			while (d-- > 0 && *aValue != '\0')
				line.insert(line.begin() + cindex++, glyph(*aValue++, pallete_index::def));
			++aWhere.m_column;
		}

		m_text_changed = true;
	}

	return totalLines;
}

void c_text_editor::add_undo(undo_record& aValue)
{
	assert(!m_read_only);
	//printf("add_undo: (@%d.%d) +\'%s' [%d.%d .. %d.%d], -\'%s', [%d.%d .. %d.%d] (@%d.%d)\n",
	//	aValue.m_before.m_cursor_position.m_line, aValue.m_before.m_cursor_position.m_column,
	//	aValue.m_added.c_str(), aValue.m_added_start.m_line, aValue.m_added_start.m_column, aValue.m_added_end.m_line, aValue.m_added_end.m_column,
	//	aValue.m_removed.c_str(), aValue.m_removed_start.m_line, aValue.m_removed_start.m_column, aValue.m_removed_end.m_line, aValue.m_removed_end.m_column,
	//	aValue.m_after.m_cursor_position.m_line, aValue.m_after.m_cursor_position.m_column
	//	);

	m_undo_buffer.resize((size_t)(m_undo_index + 1));
	m_undo_buffer.back() = aValue;
	++m_undo_index;
}

c_text_editor::coordinates c_text_editor::screen_pos_to_coordinates(const ImVec2& aPosition) const
{
	ImVec2 origin = ImGui::GetCursorScreenPos();
	ImVec2 local(aPosition.x - origin.x, aPosition.y - origin.y);

	int lineNo = max(0, (int)floor(local.y / m_char_advance.y));

	int columnCoord = 0;

	if (lineNo >= 0 && lineNo < (int)m_lines.size())
	{
		auto& line = m_lines.at(lineNo);

		int columnIndex = 0;
		float columnX = 0.0f;

		while ((size_t)columnIndex < line.size())
		{
			float columnWidth = 0.0f;

			if (line[columnIndex].m_char == '\t')
			{
				float spaceSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, " ").x;
				float oldX = columnX;
				float newColumnX = (1.0f + std::floor((1.0f + columnX) / (float(m_tab_size) * spaceSize))) * (float(m_tab_size) * spaceSize);
				columnWidth = newColumnX - oldX;
				if (m_text_start + columnX + columnWidth * 0.5f > local.x)
					break;
				columnX = newColumnX;
				columnCoord = (columnCoord / m_tab_size) * m_tab_size + m_tab_size;
				columnIndex++;
			}
			else
			{
				char buf[7];
				auto d = UTF8CharLength(line[columnIndex].m_char);
				int i = 0;
				while (i < 6 && d-- > 0)
					buf[i++] = line[columnIndex++].m_char;
				buf[i] = '\0';
				columnWidth = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buf).x;
				if (m_text_start + columnX + columnWidth * 0.5f > local.x)
					break;
				columnX += columnWidth;
				columnCoord++;
			}
		}
	}

	return sanitize_coordinates(coordinates(lineNo, columnCoord));
}

c_text_editor::coordinates c_text_editor::find_word_start(const coordinates& aFrom) const
{
	coordinates at = aFrom;
	if (at.m_line >= (int)m_lines.size())
		return at;

	auto& line = m_lines[at.m_line];
	auto cindex = get_character_index(at);

	if (cindex >= (int)line.size())
		return at;

	while (cindex > 0 && isspace(line[cindex].m_char))
		--cindex;

	auto cstart = (pallete_index)line[cindex].m_color_idex;
	while (cindex > 0)
	{
		auto c = line[cindex].m_char;
		if ((c & 0xC0) != 0x80)	// not UTF code sequence 10xxxxxx
		{
			if (c <= 32 && isspace(c))
			{
				cindex++;
				break;
			}
			if (cstart != (pallete_index)line[size_t(cindex - 1)].m_color_idex)
				break;
		}
		--cindex;
	}
	return coordinates(at.m_line, get_character_column(at.m_line, cindex));
}

c_text_editor::coordinates c_text_editor::find_word_end(const coordinates& aFrom) const
{
	coordinates at = aFrom;
	if (at.m_line >= (int)m_lines.size())
		return at;

	auto& line = m_lines[at.m_line];
	auto cindex = get_character_index(at);

	if (cindex >= (int)line.size())
		return at;

	bool prevspace = (bool)isspace(line[cindex].m_char);
	auto cstart = (pallete_index)line[cindex].m_color_idex;
	while (cindex < (int)line.size())
	{
		auto c = line[cindex].m_char;
		auto d = UTF8CharLength(c);
		if (cstart != (pallete_index)line[cindex].m_color_idex)
			break;

		if (prevspace != !!isspace(c))
		{
			if (isspace(c))
				while (cindex < (int)line.size() && isspace(line[cindex].m_char))
					++cindex;
			break;
		}
		cindex += d;
	}
	return coordinates(aFrom.m_line, get_character_column(aFrom.m_line, cindex));
}

c_text_editor::coordinates c_text_editor::find_next_word(const coordinates& aFrom) const
{
	coordinates at = aFrom;
	if (at.m_line >= (int)m_lines.size())
		return at;

	// skip to the next non-word character
	auto cindex = get_character_index(aFrom);
	bool isword = false;
	bool skip = false;
	if (cindex < (int)m_lines[at.m_line].size())
	{
		auto& line = m_lines[at.m_line];
		isword = isalnum(line[cindex].m_char);
		skip = isword;
	}

	while (!isword || skip)
	{
		if (at.m_line >= m_lines.size())
		{
			auto l = max(0, (int)m_lines.size() - 1);
			return coordinates(l, get_line_max_column(l));
		}

		auto& line = m_lines[at.m_line];
		if (cindex < (int)line.size())
		{
			isword = isalnum(line[cindex].m_char);

			if (isword && !skip)
				return coordinates(at.m_line, get_character_column(at.m_line, cindex));

			if (!isword)
				skip = false;

			cindex++;
		}
		else
		{
			cindex = 0;
			++at.m_line;
			skip = false;
			isword = false;
		}
	}

	return at;
}

int c_text_editor::get_character_index(const coordinates& aCoordinates) const
{
	if (aCoordinates.m_line >= m_lines.size())
		return -1;
	auto& line = m_lines[aCoordinates.m_line];
	int c = 0;
	int i = 0;
	for (; i < line.size() && c < aCoordinates.m_column;)
	{
		if (line[i].m_char == '\t')
			c = (c / m_tab_size) * m_tab_size + m_tab_size;
		else
			++c;
		i += UTF8CharLength(line[i].m_char);
	}
	return i;
}

int c_text_editor::get_character_column(int a_line, int aIndex) const
{
	if (a_line >= m_lines.size())
		return 0;
	auto& line = m_lines[a_line];
	int col = 0;
	int i = 0;
	while (i < aIndex && i < (int)line.size())
	{
		auto c = line[i].m_char;
		i += UTF8CharLength(c);
		if (c == '\t')
			col = (col / m_tab_size) * m_tab_size + m_tab_size;
		else
			col++;
	}
	return col;
}

int c_text_editor::get_line_character_count(int a_line) const
{
	if (a_line >= m_lines.size())
		return 0;
	auto& line = m_lines[a_line];
	int c = 0;
	for (unsigned i = 0; i < line.size(); c++)
		i += UTF8CharLength(line[i].m_char);
	return c;
}

int c_text_editor::get_line_max_column(int a_line) const
{
	if (a_line >= m_lines.size())
		return 0;
	auto& line = m_lines[a_line];
	int col = 0;
	for (unsigned i = 0; i < line.size(); )
	{
		auto c = line[i].m_char;
		if (c == '\t')
			col = (col / m_tab_size) * m_tab_size + m_tab_size;
		else
			col++;
		i += UTF8CharLength(c);
	}
	return col;
}

bool c_text_editor::is_on_word_boundary(const coordinates& aAt) const
{
	if (aAt.m_line >= (int)m_lines.size() || aAt.m_column == 0)
		return true;

	auto& line = m_lines[aAt.m_line];
	auto cindex = get_character_index(aAt);
	if (cindex >= (int)line.size())
		return true;

	if (m_colorizer_enabled)
		return line[cindex].m_color_idex != line[size_t(cindex - 1)].m_color_idex;

	return isspace(line[cindex].m_char) != isspace(line[cindex - 1].m_char);
}

void c_text_editor::remove_line(int aStart, int aEnd)
{
	assert(!m_read_only);
	assert(aEnd >= aStart);
	assert(m_lines.size() > (size_t)(aEnd - aStart));

	error_markers etmp;
	for (auto& i : m_error_markers)
	{
		error_markers::value_type e(i.first >= aStart ? i.first - 1 : i.first, i.second);
		if (e.first >= aStart && e.first <= aEnd)
			continue;
		etmp.insert(e);
	}
	m_error_markers = std::move(etmp);

	breakpoints btmp;
	for (auto i : m_breakpoints)
	{
		if (i >= aStart && i <= aEnd)
			continue;
		btmp.insert(i >= aStart ? i - 1 : i);
	}
	m_breakpoints = std::move(btmp);

	m_lines.erase(m_lines.begin() + aStart, m_lines.begin() + aEnd);
	assert(!m_lines.empty());

	m_text_changed = true;
}

void c_text_editor::remove_line(int aIndex)
{
	assert(!m_read_only);
	assert(m_lines.size() > 1);

	error_markers etmp;
	for (auto& i : m_error_markers)
	{
		error_markers::value_type e(i.first > aIndex ? i.first - 1 : i.first, i.second);
		if (e.first - 1 == aIndex)
			continue;
		etmp.insert(e);
	}
	m_error_markers = std::move(etmp);

	breakpoints btmp;
	for (auto i : m_breakpoints)
	{
		if (i == aIndex)
			continue;
		btmp.insert(i >= aIndex ? i - 1 : i);
	}
	m_breakpoints = std::move(btmp);

	m_lines.erase(m_lines.begin() + aIndex);
	assert(!m_lines.empty());

	m_text_changed = true;
}

c_text_editor::line& c_text_editor::insert_line(int aIndex)
{
	assert(!m_read_only);

	auto& result = *m_lines.insert(m_lines.begin() + aIndex, line());

	error_markers etmp;
	for (auto& i : m_error_markers)
		etmp.insert(error_markers::value_type(i.first >= aIndex ? i.first + 1 : i.first, i.second));
	m_error_markers = std::move(etmp);

	breakpoints btmp;
	for (auto i : m_breakpoints)
		btmp.insert(i >= aIndex ? i + 1 : i);
	m_breakpoints = std::move(btmp);

	return result;
}

std::string c_text_editor::get_word_under_cursor() const
{
	auto c = get_cursor_position();
	return get_word_at(c);
}

std::string c_text_editor::get_word_at(const coordinates& aCoords) const
{
	auto start = find_word_start(aCoords);
	auto end = find_word_end(aCoords);

	std::string r;

	auto istart = get_character_index(start);
	auto iend = get_character_index(end);

	for (auto it = istart; it < iend; ++it)
		r.push_back(m_lines[aCoords.m_line][it].m_char);

	return r;
}

ImU32 c_text_editor::get_glyph_color(const glyph& aGlyph) const
{
	if (!m_colorizer_enabled)
		return m_pallete[(int)pallete_index::def];
	if (aGlyph.m_comment)
		return m_pallete[(int)pallete_index::comment];
	if (aGlyph.m_multiline_comment)
		return m_pallete[(int)pallete_index::multi_line_comment];
	auto const color = m_pallete[(int)aGlyph.m_color_idex];
	if (aGlyph.m_preprocessor)
	{
		const auto ppcolor = m_pallete[(int)pallete_index::preprocessor];
		const int c0 = ((ppcolor & 0xff) + (color & 0xff)) / 2;
		const int c1 = (((ppcolor >> 8) & 0xff) + ((color >> 8) & 0xff)) / 2;
		const int c2 = (((ppcolor >> 16) & 0xff) + ((color >> 16) & 0xff)) / 2;
		const int c3 = (((ppcolor >> 24) & 0xff) + ((color >> 24) & 0xff)) / 2;
		return ImU32(c0 | (c1 << 8) | (c2 << 16) | (c3 << 24));
	}
	return color;
}

void c_text_editor::handle_keyboard_inputs()
{
	ImGuiIO& io = ImGui::GetIO();
	auto shift = io.KeyShift;
	auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
	auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

	if (ImGui::IsWindowFocused())
	{
		if (ImGui::IsWindowHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
		//ImGui::CaptureKeyboardFromApp(true);

		io.WantCaptureKeyboard = true;
		io.WantTextInput = true;

		if (!is_read_only() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
			undo();
		else if (!is_read_only() && !ctrl && !shift && alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)))
			undo();
		else if (!is_read_only() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Y)))
			redo();
		else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)))
			move_up(1, shift);
		else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)))
			move_down(1, shift);
		else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)))
			move_left(1, shift, ctrl);
		else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)))
			move_right(1, shift, ctrl);
		else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageUp)))
			move_up(get_page_size() - 4, shift);
		else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageDown)))
			move_down(get_page_size() - 4, shift);
		else if (!alt && ctrl && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Home)))
			move_top(shift);
		else if (ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_End)))
			move_bottom(shift);
		else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Home)))
			move_home(shift);
		else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_End)))
			move_end(shift);
		else if (!is_read_only() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
			mdelete();
		else if (!is_read_only() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)))
			backspace();
		else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
			m_overwrite ^= true;
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
			copy();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C)))
			copy();
		else if (!is_read_only() && !ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
			paste();
		else if (!is_read_only() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_V)))
			paste();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_X)))
			cut();
		else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
			cut();
		else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
			select_all();
		else if (!is_read_only() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
			enter_character('\n', false);
		else if (!is_read_only() && !ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Tab)))
			enter_character('\t', shift);

		if (!is_read_only() && !io.InputQueueCharacters.empty())
		{
			for (int i = 0; i < io.InputQueueCharacters.Size; i++)
			{
				auto c = io.InputQueueCharacters[i];
				if (c != 0 && (c == '\n' || c >= 32))
					enter_character(c, shift);
			}
			io.InputQueueCharacters.resize(0);
		}
	}
}

void c_text_editor::handle_mouse_inputs()
{
	ImGuiIO& io = ImGui::GetIO();
	auto shift = io.KeyShift;
	auto ctrl = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
	auto alt = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

	if (ImGui::IsWindowHovered())
	{
		if (!shift && !alt)
		{
			auto click = ImGui::IsMouseClicked(0);
			auto doubleClick = ImGui::IsMouseDoubleClicked(0);
			auto t = ImGui::GetTime();
			auto tripleClick = click && !doubleClick && (m_last_click != -1.0f && (t - m_last_click) < io.MouseDoubleClickTime);

			/*
			Left mouse button triple click
			*/

			if (tripleClick)
			{
				if (!ctrl)
				{
					m_state.m_cursor_position = m_interactive_start = m_interactive_end = screen_pos_to_coordinates(ImGui::GetMousePos());
					m_selection_mode = selection_mode::line;
					set_selection(m_interactive_start, m_interactive_end, m_selection_mode);
				}

				m_last_click = -1.0f;
			}

			/*
			Left mouse button double click
			*/

			else if (doubleClick)
			{
				if (!ctrl)
				{
					m_state.m_cursor_position = m_interactive_start = m_interactive_end = screen_pos_to_coordinates(ImGui::GetMousePos());
					if (m_selection_mode == selection_mode::line)
						m_selection_mode = selection_mode::normal;
					else
						m_selection_mode = selection_mode::word;
					set_selection(m_interactive_start, m_interactive_end, m_selection_mode);
				}

				m_last_click = (float)ImGui::GetTime();
			}

			/*
			Left mouse button click
			*/
			else if (click)
			{
				m_state.m_cursor_position = m_interactive_start = m_interactive_end = screen_pos_to_coordinates(ImGui::GetMousePos());
				if (ctrl)
					m_selection_mode = selection_mode::word;
				else
					m_selection_mode = selection_mode::normal;
				set_selection(m_interactive_start, m_interactive_end, m_selection_mode);

				m_last_click = (float)ImGui::GetTime();
			}
			// Mouse left button dragging (=> update selection)
			else if (ImGui::IsMouseDragging(0) && ImGui::IsMouseDown(0))
			{
				io.WantCaptureMouse = true;
				m_state.m_cursor_position = m_interactive_end = screen_pos_to_coordinates(ImGui::GetMousePos());
				set_selection(m_interactive_start, m_interactive_end, m_selection_mode);
			}
		}
	}
}

void c_text_editor::render()
{
	/* Compute m_char_advance regarding to scaled font size (Ctrl + mouse wheel)*/
	const float fontSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, "#", nullptr, nullptr).x;
	m_char_advance = ImVec2(fontSize, ImGui::GetTextLineHeightWithSpacing() * m_line_spacing);

	/* Update palette with the current alpha from style */
	for (int i = 0; i < (int)pallete_index::max; ++i)
	{
		auto color = ImGui::ColorConvertU32ToFloat4(m_pallete_base[i]);
		color.w *= ImGui::GetStyle().Alpha;
		m_pallete[i] = ImGui::ColorConvertFloat4ToU32(color);
	}

	assert(m_line_buffer.empty());

	auto contentSize = ImGui::GetWindowContentRegionMax();
	auto drawList = ImGui::GetWindowDrawList();
	float longest(m_text_start);

	if (m_scroll_to_top)
	{
		m_scroll_to_top = false;
		ImGui::SetScrollY(0.f);
	}

	ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
	auto scrollX = ImGui::GetScrollX();
	auto scrollY = ImGui::GetScrollY();

	auto lineNo = (int)floor(scrollY / m_char_advance.y);
	auto globalLineMax = (int)m_lines.size();
	auto lineMax = max(0, min((int)m_lines.size() - 1, lineNo + (int)floor((scrollY + contentSize.y) / m_char_advance.y)));

	// Deduce m_text_start by evaluating m_lines size (global lineMax) plus two spaces as text width
	char buf[16];
	snprintf(buf, 16, " %d ", globalLineMax);
	m_text_start = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buf, nullptr, nullptr).x + SCALE(m_left_margin);

	if (!m_lines.empty())
	{
		float spaceSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, " ", nullptr, nullptr).x;

		while (lineNo <= lineMax)
		{
			ImVec2 lineStartScreenPos = ImVec2(cursorScreenPos.x, cursorScreenPos.y + lineNo * m_char_advance.y);
			ImVec2 textScreenPos = ImVec2(lineStartScreenPos.x + m_text_start, lineStartScreenPos.y);

			auto& line = m_lines[lineNo];
			auto columnNo = 0;
			coordinates lineStartCoord(lineNo, 0);
			coordinates lineEndCoord(lineNo, get_line_max_column(lineNo));

			// Draw selection for the current line
			float sstart = -1.0f;
			float ssend = -1.0f;

			assert(m_state.m_selection_start <= m_state.m_selection_end);
			if (m_state.m_selection_start <= lineEndCoord)
				sstart = m_state.m_selection_start > lineStartCoord ? text_distance_to_line_start(m_state.m_selection_start) : 0.0f;
			if (m_state.m_selection_end > lineStartCoord)
				ssend = text_distance_to_line_start(m_state.m_selection_end < lineEndCoord ? m_state.m_selection_end : lineEndCoord);

			if (m_state.m_selection_end.m_line > lineNo)
				ssend += m_char_advance.x;

			if (sstart != -1 && ssend != -1 && sstart < ssend)
			{
				ImVec2 vstart(lineStartScreenPos.x + m_text_start + sstart, lineStartScreenPos.y);
				ImVec2 vend(lineStartScreenPos.x + m_text_start + ssend, lineStartScreenPos.y + m_char_advance.y);
				drawList->AddRectFilled(vstart, vend, m_pallete[(int)pallete_index::selection], SCALE(set->c_child.rounding));
			}

			// Draw breakpoints
			auto start = ImVec2(lineStartScreenPos.x + scrollX, lineStartScreenPos.y);

			if (m_breakpoints.count(lineNo + 1) != 0)
			{
				auto end = ImVec2(lineStartScreenPos.x + contentSize.x + 2.0f * scrollX, lineStartScreenPos.y + m_char_advance.y);
				drawList->AddRectFilled(start, end, m_pallete[(int)pallete_index::breakpoint]);
			}

			// Draw error markers
			auto errorIt = m_error_markers.find(lineNo + 1);
			if (errorIt != m_error_markers.end())
			{
				auto end = ImVec2(lineStartScreenPos.x + contentSize.x + 2.0f * scrollX, lineStartScreenPos.y + m_char_advance.y);
				drawList->AddRectFilled(start, end, m_pallete[(int)pallete_index::error_marker]);

				if (ImGui::IsMouseHoveringRect(lineStartScreenPos, end))
				{
					ImGui::BeginTooltip();
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
					ImGui::Text("Error at line %d:", errorIt->first);
					ImGui::PopStyleColor();
					ImGui::Separator();
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.2f, 1.0f));
					ImGui::Text("%s", errorIt->second.c_str());
					ImGui::PopStyleColor();
					ImGui::EndTooltip();
				}
			}

			// Draw line number (right aligned)
			snprintf(buf, 16, "%d  ", lineNo + 1);

			auto lineNoWidth = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buf, nullptr, nullptr).x;
			drawList->AddText(ImVec2(lineStartScreenPos.x + m_text_start - lineNoWidth, lineStartScreenPos.y), m_pallete[(int)pallete_index::line_number], buf);

			if (m_state.m_cursor_position.m_line == lineNo)
			{
				auto focused = ImGui::IsWindowFocused();

				// render the cursor
				if (focused)
				{
					auto timeEnd = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					auto elapsed = timeEnd - m_start_time;
					if (elapsed > 400)
					{
						float width = 1.0f;
						auto cindex = get_character_index(m_state.m_cursor_position);
						float cx = text_distance_to_line_start(m_state.m_cursor_position);

						if (m_overwrite && cindex < (int)line.size())
						{
							auto c = line[cindex].m_char;
							if (c == '\t')
							{
								auto x = (1.0f + std::floor((1.0f + cx) / (float(m_tab_size) * spaceSize))) * (float(m_tab_size) * spaceSize);
								width = x - cx;
							}
							else
							{
								char buf2[2];
								buf2[0] = line[cindex].m_char;
								buf2[1] = '\0';
								width = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buf2).x;
							}
						}
						ImVec2 cstart(textScreenPos.x + cx, lineStartScreenPos.y);
						ImVec2 cend(textScreenPos.x + cx + width, lineStartScreenPos.y + m_char_advance.y);
						drawList->AddRectFilled(cstart, cend, m_pallete[(int)pallete_index::cursor]);
						if (elapsed > 800)
							m_start_time = timeEnd;
					}
				}
			}

			// render colorized text
			auto prevColor = line.empty() ? m_pallete[(int)pallete_index::def] : get_glyph_color(line[0]);
			ImVec2 bufferOffset;

			for (int i = 0; i < line.size();)
			{
				auto& glyph = line[i];
				auto color = get_glyph_color(glyph);

				if ((color != prevColor || glyph.m_char == '\t' || glyph.m_char == ' ') && !m_line_buffer.empty())
				{
					const ImVec2 newOffset(textScreenPos.x + bufferOffset.x, textScreenPos.y + bufferOffset.y);
					drawList->AddText(newOffset, prevColor, m_line_buffer.c_str());
					auto textSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, m_line_buffer.c_str(), nullptr, nullptr);
					bufferOffset.x += textSize.x;
					m_line_buffer.clear();
				}
				prevColor = color;

				if (glyph.m_char == '\t')
				{
					auto oldX = bufferOffset.x;
					bufferOffset.x = (1.0f + std::floor((1.0f + bufferOffset.x) / (float(m_tab_size) * spaceSize))) * (float(m_tab_size) * spaceSize);
					++i;

					if (m_show_whitespaces)
					{
						const auto s = ImGui::GetFontSize();
						const auto x1 = textScreenPos.x + oldX + 1.0f;
						const auto x2 = textScreenPos.x + bufferOffset.x - 1.0f;
						const auto y = textScreenPos.y + bufferOffset.y + s * 0.5f;
						const ImVec2 p1(x1, y);
						const ImVec2 p2(x2, y);
						const ImVec2 p3(x2 - s * 0.2f, y - s * 0.2f);
						const ImVec2 p4(x2 - s * 0.2f, y + s * 0.2f);
						drawList->AddLine(p1, p2, 0x90909090);
						drawList->AddLine(p2, p3, 0x90909090);
						drawList->AddLine(p2, p4, 0x90909090);
					}
				}
				else if (glyph.m_char == ' ')
				{
					bufferOffset.x += spaceSize;
					i++;
				}
				else
				{
					auto l = UTF8CharLength(glyph.m_char);
					while (l-- > 0)
						m_line_buffer.push_back(line[i++].m_char);
				}
				++columnNo;
			}

			if (!m_line_buffer.empty())
			{
				const ImVec2 newOffset(textScreenPos.x + bufferOffset.x, textScreenPos.y + bufferOffset.y);
				drawList->AddText(newOffset, prevColor, m_line_buffer.c_str());
				m_line_buffer.clear();
			}

			++lineNo;
		}
	}


	ImGui::Dummy(ImVec2((longest + 2), m_lines.size() * m_char_advance.y));

	if (m_scroll_to_cursor)
	{
		ensure_cursor_visible();
		ImGui::SetWindowFocus();
		m_scroll_to_cursor = false;
	}
}

void c_text_editor::render(const char* aTitle, const ImVec2& aSize, bool aBorder)
{
	m_within_render = true;
	m_text_changed = false;
	m_cursor_position_changed = false;
	
	if (!m_ignore_imgui_child)
		gui->begin_content(aTitle, aSize, SCALE(0, 5), SCALE(0, 0));

	if (m_handle_keyboard_inputs)
	{
		handle_keyboard_inputs();
		ImGui::PushAllowKeyboardFocus(true);
	}

	if (m_handle_mouse_inputs)
		handle_mouse_inputs();

	colorize_internal();
	render();

	if (m_handle_keyboard_inputs)
		ImGui::PopAllowKeyboardFocus();

	if (!m_ignore_imgui_child)
		gui->end_content();

	m_within_render = false;
}

void c_text_editor::set_text(const std::string& aText)
{
	m_lines.clear();
	m_lines.emplace_back(line());
	for (auto chr : aText)
	{
		if (chr == '\r')
		{
			// ignore the carriage return character
		}
		else if (chr == '\n')
			m_lines.emplace_back(line());
		else
		{
			m_lines.back().emplace_back(glyph(chr, pallete_index::def));
		}
	}

	m_text_changed = true;
	m_scroll_to_top = true;

	m_undo_buffer.clear();
	m_undo_index = 0;

	colorize();
}

void c_text_editor::set_text_lines(const std::vector<std::string>& aLines)
{
	m_lines.clear();

	if (aLines.empty())
	{
		m_lines.emplace_back(line());
	}
	else
	{
		m_lines.resize(aLines.size());

		for (size_t i = 0; i < aLines.size(); ++i)
		{
			const std::string& a_line = aLines[i];

			m_lines[i].reserve(a_line.size());
			for (size_t j = 0; j < a_line.size(); ++j)
				m_lines[i].emplace_back(glyph(a_line[j], pallete_index::def));
		}
	}

	m_text_changed = true;
	m_scroll_to_top = true;

	m_undo_buffer.clear();
	m_undo_index = 0;

	colorize();
}

void c_text_editor::enter_character(ImWchar a_char, bool aShift)
{
	assert(!m_read_only);

	undo_record u;

	u.m_before = m_state;

	if (has_selection())
	{
		if (a_char == '\t' && m_state.m_selection_start.m_line != m_state.m_selection_end.m_line)
		{

			auto start = m_state.m_selection_start;
			auto end = m_state.m_selection_end;
			auto originalEnd = end;

			if (start > end)
				std::swap(start, end);
			start.m_column = 0;
			//			end.m_column = end.m_line < m_lines.size() ? m_lines[end.m_line].size() : 0;
			if (end.m_column == 0 && end.m_line > 0)
				--end.m_line;
			if (end.m_line >= (int)m_lines.size())
				end.m_line = m_lines.empty() ? 0 : (int)m_lines.size() - 1;
			end.m_column = get_line_max_column(end.m_line);

			//if (end.m_column >= get_line_max_column(end.m_line))
			//	end.m_column = get_line_max_column(end.m_line) - 1;

			u.m_removed_start = start;
			u.m_removed_end = end;
			u.m_removed = get_text(start, end);

			bool modified = false;

			for (int i = start.m_line; i <= end.m_line; i++)
			{
				auto& line = m_lines[i];
				if (aShift)
				{
					if (!line.empty())
					{
						if (line.front().m_char == '\t')
						{
							line.erase(line.begin());
							modified = true;
						}
						else
						{
							for (int j = 0; j < m_tab_size && !line.empty() && line.front().m_char == ' '; j++)
							{
								line.erase(line.begin());
								modified = true;
							}
						}
					}
				}
				else
				{
					line.insert(line.begin(), glyph('\t', c_text_editor::pallete_index::background));
					modified = true;
				}
			}

			if (modified)
			{
				start = coordinates(start.m_line, get_character_column(start.m_line, 0));
				coordinates rangeEnd;
				if (originalEnd.m_column != 0)
				{
					end = coordinates(end.m_line, get_line_max_column(end.m_line));
					rangeEnd = end;
					u.m_added = get_text(start, end);
				}
				else
				{
					end = coordinates(originalEnd.m_line, 0);
					rangeEnd = coordinates(end.m_line - 1, get_line_max_column(end.m_line - 1));
					u.m_added = get_text(start, rangeEnd);
				}

				u.m_added_start = start;
				u.m_added_end = rangeEnd;
				u.m_after = m_state;

				m_state.m_selection_start = start;
				m_state.m_selection_end = end;
				add_undo(u);

				m_text_changed = true;

				ensure_cursor_visible();
			}

			return;
		} // c == '\t'
		else
		{
			u.m_removed = get_selected_text();
			u.m_removed_start = m_state.m_selection_start;
			u.m_removed_end = m_state.m_selection_end;
			delete_selection();
		}
	} // has_selection

	auto coord = get_actual_cursor_coordinates();
	u.m_added_start = coord;

	assert(!m_lines.empty());

	if (a_char == '\n')
	{
		insert_line(coord.m_line + 1);
		auto& line = m_lines[coord.m_line];
		auto& newLine = m_lines[coord.m_line + 1];

		if (m_language_definition.m_auto_indentation)
			for (size_t it = 0; it < line.size() && isascii(line[it].m_char) && isblank(line[it].m_char); ++it)
				newLine.push_back(line[it]);

		const size_t whitespaceSize = newLine.size();
		auto cindex = get_character_index(coord);
		newLine.insert(newLine.end(), line.begin() + cindex, line.end());
		line.erase(line.begin() + cindex, line.begin() + line.size());
		set_cursor_position(coordinates(coord.m_line + 1, get_character_column(coord.m_line + 1, (int)whitespaceSize)));
		u.m_added = (char)a_char;
	}
	else
	{
		char buf[7];
		int e = ImTextCharToUtf8(buf, 7, a_char);
		if (e > 0)
		{
			buf[e] = '\0';
			auto& line = m_lines[coord.m_line];
			auto cindex = get_character_index(coord);

			if (m_overwrite && cindex < (int)line.size())
			{
				auto d = UTF8CharLength(line[cindex].m_char);

				u.m_removed_start = m_state.m_cursor_position;
				u.m_removed_end = coordinates(coord.m_line, get_character_column(coord.m_line, cindex + d));

				while (d-- > 0 && cindex < (int)line.size())
				{
					u.m_removed += line[cindex].m_char;
					line.erase(line.begin() + cindex);
				}
			}

			for (auto p = buf; *p != '\0'; p++, ++cindex)
				line.insert(line.begin() + cindex, glyph(*p, pallete_index::def));
			u.m_added = buf;

			set_cursor_position(coordinates(coord.m_line, get_character_column(coord.m_line, cindex)));
		}
		else
			return;
	}

	m_text_changed = true;

	u.m_added_end = get_actual_cursor_coordinates();
	u.m_after = m_state;

	add_undo(u);

	colorize(coord.m_line - 1, 3);
	ensure_cursor_visible();
}

void c_text_editor::set_read_only(bool aValue)
{
	m_read_only = aValue;
}

void c_text_editor::set_colorizer_enable(bool aValue)
{
	m_colorizer_enabled = aValue;
}

void c_text_editor::set_cursor_position(const coordinates& aPosition)
{
	if (m_state.m_cursor_position != aPosition)
	{
		m_state.m_cursor_position = aPosition;
		m_cursor_position_changed = true;
		ensure_cursor_visible();
	}
}

void c_text_editor::set_selection_start(const coordinates& aPosition)
{
	m_state.m_selection_start = sanitize_coordinates(aPosition);
	if (m_state.m_selection_start > m_state.m_selection_end)
		std::swap(m_state.m_selection_start, m_state.m_selection_end);
}

void c_text_editor::set_selection_end(const coordinates& aPosition)
{
	m_state.m_selection_end = sanitize_coordinates(aPosition);
	if (m_state.m_selection_start > m_state.m_selection_end)
		std::swap(m_state.m_selection_start, m_state.m_selection_end);
}

void c_text_editor::set_selection(const coordinates& aStart, const coordinates& aEnd, selection_mode aMode)
{
	auto oldSelStart = m_state.m_selection_start;
	auto oldSelEnd = m_state.m_selection_end;

	m_state.m_selection_start = sanitize_coordinates(aStart);
	m_state.m_selection_end = sanitize_coordinates(aEnd);
	if (m_state.m_selection_start > m_state.m_selection_end)
		std::swap(m_state.m_selection_start, m_state.m_selection_end);

	switch (aMode)
	{
	case c_text_editor::selection_mode::normal:
		break;
	case c_text_editor::selection_mode::word:
	{
		m_state.m_selection_start = find_word_start(m_state.m_selection_start);
		if (!is_on_word_boundary(m_state.m_selection_end))
			m_state.m_selection_end = find_word_end(find_word_start(m_state.m_selection_end));
		break;
	}
	case c_text_editor::selection_mode::line:
	{
		const auto lineNo = m_state.m_selection_end.m_line;
		const auto lineSize = (size_t)lineNo < m_lines.size() ? m_lines[lineNo].size() : 0;
		m_state.m_selection_start = coordinates(m_state.m_selection_start.m_line, 0);
		m_state.m_selection_end = coordinates(lineNo, get_line_max_column(lineNo));
		break;
	}
	default:
		break;
	}

	if (m_state.m_selection_start != oldSelStart ||
		m_state.m_selection_end != oldSelEnd)
		m_cursor_position_changed = true;
}

void c_text_editor::set_tab_size(int aValue)
{
	m_tab_size = max(0, min(32, aValue));
}

void c_text_editor::insert_text(const std::string& aValue)
{
	insert_text(aValue.c_str());
}

void c_text_editor::insert_text(const char* aValue)
{
	if (aValue == nullptr)
		return;

	auto pos = get_actual_cursor_coordinates();
	auto start = min(pos, m_state.m_selection_start);
	int totalLines = pos.m_line - start.m_line;

	totalLines += insert_text_at(pos, aValue);

	set_selection(pos, pos);
	set_cursor_position(pos);
	colorize(start.m_line - 1, totalLines + 2);
}

void c_text_editor::delete_selection()
{
	assert(m_state.m_selection_end >= m_state.m_selection_start);

	if (m_state.m_selection_end == m_state.m_selection_start)
		return;

	delete_range(m_state.m_selection_start, m_state.m_selection_end);

	set_selection(m_state.m_selection_start, m_state.m_selection_start);
	set_cursor_position(m_state.m_selection_start);
	colorize(m_state.m_selection_start.m_line, 1);
}

void c_text_editor::move_up(int aAmount, bool aSelect)
{
	auto oldPos = m_state.m_cursor_position;
	m_state.m_cursor_position.m_line = max(0, m_state.m_cursor_position.m_line - aAmount);
	if (oldPos != m_state.m_cursor_position)
	{
		if (aSelect)
		{
			if (oldPos == m_interactive_start)
				m_interactive_start = m_state.m_cursor_position;
			else if (oldPos == m_interactive_end)
				m_interactive_end = m_state.m_cursor_position;
			else
			{
				m_interactive_start = m_state.m_cursor_position;
				m_interactive_end = oldPos;
			}
		}
		else
			m_interactive_start = m_interactive_end = m_state.m_cursor_position;
		set_selection(m_interactive_start, m_interactive_end);

		ensure_cursor_visible();
	}
}

void c_text_editor::move_down(int aAmount, bool aSelect)
{
	assert(m_state.m_cursor_position.m_column >= 0);
	auto oldPos = m_state.m_cursor_position;
	m_state.m_cursor_position.m_line = max(0, min((int)m_lines.size() - 1, m_state.m_cursor_position.m_line + aAmount));

	if (m_state.m_cursor_position != oldPos)
	{
		if (aSelect)
		{
			if (oldPos == m_interactive_end)
				m_interactive_end = m_state.m_cursor_position;
			else if (oldPos == m_interactive_start)
				m_interactive_start = m_state.m_cursor_position;
			else
			{
				m_interactive_start = oldPos;
				m_interactive_end = m_state.m_cursor_position;
			}
		}
		else
			m_interactive_start = m_interactive_end = m_state.m_cursor_position;
		set_selection(m_interactive_start, m_interactive_end);

		ensure_cursor_visible();
	}
}

static bool IsUTFSequence(char c)
{
	return (c & 0xC0) == 0x80;
}

void c_text_editor::move_left(int aAmount, bool aSelect, bool aWordMode)
{
	if (m_lines.empty())
		return;

	auto oldPos = m_state.m_cursor_position;
	m_state.m_cursor_position = get_actual_cursor_coordinates();
	auto line = m_state.m_cursor_position.m_line;
	auto cindex = get_character_index(m_state.m_cursor_position);

	while (aAmount-- > 0)
	{
		if (cindex == 0)
		{
			if (line > 0)
			{
				--line;
				if ((int)m_lines.size() > line)
					cindex = (int)m_lines[line].size();
				else
					cindex = 0;
			}
		}
		else
		{
			--cindex;
			if (cindex > 0)
			{
				if ((int)m_lines.size() > line)
				{
					while (cindex > 0 && IsUTFSequence(m_lines[line][cindex].m_char))
						--cindex;
				}
			}
		}

		m_state.m_cursor_position = coordinates(line, get_character_column(line, cindex));
		if (aWordMode)
		{
			m_state.m_cursor_position = find_word_start(m_state.m_cursor_position);
			cindex = get_character_index(m_state.m_cursor_position);
		}
	}

	m_state.m_cursor_position = coordinates(line, get_character_column(line, cindex));

	assert(m_state.m_cursor_position.m_column >= 0);
	if (aSelect)
	{
		if (oldPos == m_interactive_start)
			m_interactive_start = m_state.m_cursor_position;
		else if (oldPos == m_interactive_end)
			m_interactive_end = m_state.m_cursor_position;
		else
		{
			m_interactive_start = m_state.m_cursor_position;
			m_interactive_end = oldPos;
		}
	}
	else
		m_interactive_start = m_interactive_end = m_state.m_cursor_position;
	set_selection(m_interactive_start, m_interactive_end, aSelect && aWordMode ? selection_mode::word : selection_mode::normal);

	ensure_cursor_visible();
}

void c_text_editor::move_right(int aAmount, bool aSelect, bool aWordMode)
{
	auto oldPos = m_state.m_cursor_position;

	if (m_lines.empty() || oldPos.m_line >= m_lines.size())
		return;

	auto cindex = get_character_index(m_state.m_cursor_position);
	while (aAmount-- > 0)
	{
		auto lindex = m_state.m_cursor_position.m_line;
		auto& line = m_lines[lindex];

		if (cindex >= line.size())
		{
			if (m_state.m_cursor_position.m_line < m_lines.size() - 1)
			{
				m_state.m_cursor_position.m_line = max(0, min((int)m_lines.size() - 1, m_state.m_cursor_position.m_line + 1));
				m_state.m_cursor_position.m_column = 0;
			}
			else
				return;
		}
		else
		{
			cindex += UTF8CharLength(line[cindex].m_char);
			m_state.m_cursor_position = coordinates(lindex, get_character_column(lindex, cindex));
			if (aWordMode)
				m_state.m_cursor_position = find_next_word(m_state.m_cursor_position);
		}
	}

	if (aSelect)
	{
		if (oldPos == m_interactive_end)
			m_interactive_end = sanitize_coordinates(m_state.m_cursor_position);
		else if (oldPos == m_interactive_start)
			m_interactive_start = m_state.m_cursor_position;
		else
		{
			m_interactive_start = oldPos;
			m_interactive_end = m_state.m_cursor_position;
		}
	}
	else
		m_interactive_start = m_interactive_end = m_state.m_cursor_position;
	set_selection(m_interactive_start, m_interactive_end, aSelect && aWordMode ? selection_mode::word : selection_mode::normal);

	ensure_cursor_visible();
}

void c_text_editor::move_top(bool aSelect)
{
	auto oldPos = m_state.m_cursor_position;
	set_cursor_position(coordinates(0, 0));

	if (m_state.m_cursor_position != oldPos)
	{
		if (aSelect)
		{
			m_interactive_end = oldPos;
			m_interactive_start = m_state.m_cursor_position;
		}
		else
			m_interactive_start = m_interactive_end = m_state.m_cursor_position;
		set_selection(m_interactive_start, m_interactive_end);
	}
}

void c_text_editor::c_text_editor::move_bottom(bool aSelect)
{
	auto oldPos = get_cursor_position();
	auto newPos = coordinates((int)m_lines.size() - 1, 0);
	set_cursor_position(newPos);
	if (aSelect)
	{
		m_interactive_start = oldPos;
		m_interactive_end = newPos;
	}
	else
		m_interactive_start = m_interactive_end = newPos;
	set_selection(m_interactive_start, m_interactive_end);
}

void c_text_editor::move_home(bool aSelect)
{
	auto oldPos = m_state.m_cursor_position;
	set_cursor_position(coordinates(m_state.m_cursor_position.m_line, 0));

	if (m_state.m_cursor_position != oldPos)
	{
		if (aSelect)
		{
			if (oldPos == m_interactive_start)
				m_interactive_start = m_state.m_cursor_position;
			else if (oldPos == m_interactive_end)
				m_interactive_end = m_state.m_cursor_position;
			else
			{
				m_interactive_start = m_state.m_cursor_position;
				m_interactive_end = oldPos;
			}
		}
		else
			m_interactive_start = m_interactive_end = m_state.m_cursor_position;
		set_selection(m_interactive_start, m_interactive_end);
	}
}

void c_text_editor::move_end(bool aSelect)
{
	auto oldPos = m_state.m_cursor_position;
	set_cursor_position(coordinates(m_state.m_cursor_position.m_line, get_line_max_column(oldPos.m_line)));

	if (m_state.m_cursor_position != oldPos)
	{
		if (aSelect)
		{
			if (oldPos == m_interactive_end)
				m_interactive_end = m_state.m_cursor_position;
			else if (oldPos == m_interactive_start)
				m_interactive_start = m_state.m_cursor_position;
			else
			{
				m_interactive_start = oldPos;
				m_interactive_end = m_state.m_cursor_position;
			}
		}
		else
			m_interactive_start = m_interactive_end = m_state.m_cursor_position;
		set_selection(m_interactive_start, m_interactive_end);
	}
}

void c_text_editor::mdelete()
{
	assert(!m_read_only);

	if (m_lines.empty())
		return;

	undo_record u;
	u.m_before = m_state;

	if (has_selection())
	{
		u.m_removed = get_selected_text();
		u.m_removed_start = m_state.m_selection_start;
		u.m_removed_end = m_state.m_selection_end;

		delete_selection();
	}
	else
	{
		auto pos = get_actual_cursor_coordinates();
		set_cursor_position(pos);
		auto& line = m_lines[pos.m_line];

		if (pos.m_column == get_line_max_column(pos.m_line))
		{
			if (pos.m_line == (int)m_lines.size() - 1)
				return;

			u.m_removed = '\n';
			u.m_removed_start = u.m_removed_end = get_actual_cursor_coordinates();
			advance(u.m_removed_end);

			auto& nextLine = m_lines[pos.m_line + 1];
			line.insert(line.end(), nextLine.begin(), nextLine.end());
			remove_line(pos.m_line + 1);
		}
		else
		{
			auto cindex = get_character_index(pos);
			u.m_removed_start = u.m_removed_end = get_actual_cursor_coordinates();
			u.m_removed_end.m_column++;
			u.m_removed = get_text(u.m_removed_start, u.m_removed_end);

			auto d = UTF8CharLength(line[cindex].m_char);
			while (d-- > 0 && cindex < (int)line.size())
				line.erase(line.begin() + cindex);
		}

		m_text_changed = true;

		colorize(pos.m_line, 1);
	}

	u.m_after = m_state;
	add_undo(u);
}

void c_text_editor::backspace()
{
	assert(!m_read_only);

	if (m_lines.empty())
		return;

	undo_record u;
	u.m_before = m_state;

	if (has_selection())
	{
		u.m_removed = get_selected_text();
		u.m_removed_start = m_state.m_selection_start;
		u.m_removed_end = m_state.m_selection_end;

		delete_selection();
	}
	else
	{
		auto pos = get_actual_cursor_coordinates();
		set_cursor_position(pos);

		if (m_state.m_cursor_position.m_column == 0)
		{
			if (m_state.m_cursor_position.m_line == 0)
				return;

			u.m_removed = '\n';
			u.m_removed_start = u.m_removed_end = coordinates(pos.m_line - 1, get_line_max_column(pos.m_line - 1));
			advance(u.m_removed_end);

			auto& line = m_lines[m_state.m_cursor_position.m_line];
			auto& prevLine = m_lines[m_state.m_cursor_position.m_line - 1];
			auto prevSize = get_line_max_column(m_state.m_cursor_position.m_line - 1);
			prevLine.insert(prevLine.end(), line.begin(), line.end());

			error_markers etmp;
			for (auto& i : m_error_markers)
				etmp.insert(error_markers::value_type(i.first - 1 == m_state.m_cursor_position.m_line ? i.first - 1 : i.first, i.second));
			m_error_markers = std::move(etmp);

			remove_line(m_state.m_cursor_position.m_line);
			--m_state.m_cursor_position.m_line;
			m_state.m_cursor_position.m_column = prevSize;
		}
		else
		{
			auto& line = m_lines[m_state.m_cursor_position.m_line];
			auto cindex = get_character_index(pos) - 1;
			auto cend = cindex + 1;
			while (cindex > 0 && IsUTFSequence(line[cindex].m_char))
				--cindex;

			//if (cindex > 0 && UTF8CharLength(line[cindex].m_char) > 1)
			//	--cindex;

			u.m_removed_start = u.m_removed_end = get_actual_cursor_coordinates();
			--u.m_removed_start.m_column;
			--m_state.m_cursor_position.m_column;

			while (cindex < line.size() && cend-- > cindex)
			{
				u.m_removed += line[cindex].m_char;
				line.erase(line.begin() + cindex);
			}
		}

		m_text_changed = true;

		ensure_cursor_visible();
		colorize(m_state.m_cursor_position.m_line, 1);
	}

	u.m_after = m_state;
	add_undo(u);
}

void c_text_editor::select_word_under_cursor()
{
	auto c = get_cursor_position();
	set_selection(find_word_start(c), find_word_end(c));
}

void c_text_editor::select_all()
{
	set_selection(coordinates(0, 0), coordinates((int)m_lines.size(), 0));
}

bool c_text_editor::has_selection() const
{
	return m_state.m_selection_end > m_state.m_selection_start;
}

void c_text_editor::copy()
{
	if (has_selection())
	{
		ImGui::SetClipboardText(get_selected_text().c_str());
	}
	else
	{
		if (!m_lines.empty())
		{
			std::string str;
			auto& line = m_lines[get_actual_cursor_coordinates().m_line];
			for (auto& g : line)
				str.push_back(g.m_char);
			ImGui::SetClipboardText(str.c_str());
		}
	}
}

void c_text_editor::cut()
{
	if (is_read_only())
	{
		copy();
	}
	else
	{
		if (has_selection())
		{
			undo_record u;
			u.m_before = m_state;
			u.m_removed = get_selected_text();
			u.m_removed_start = m_state.m_selection_start;
			u.m_removed_end = m_state.m_selection_end;

			copy();
			delete_selection();

			u.m_after = m_state;
			add_undo(u);
		}
	}
}

void c_text_editor::paste()
{
	if (is_read_only())
		return;

	auto clipText = ImGui::GetClipboardText();
	if (clipText != nullptr && strlen(clipText) > 0)
	{
		undo_record u;
		u.m_before = m_state;

		if (has_selection())
		{
			u.m_removed = get_selected_text();
			u.m_removed_start = m_state.m_selection_start;
			u.m_removed_end = m_state.m_selection_end;
			delete_selection();
		}

		u.m_added = clipText;
		u.m_added_start = get_actual_cursor_coordinates();

		insert_text(clipText);

		u.m_added_end = get_actual_cursor_coordinates();
		u.m_after = m_state;
		add_undo(u);
	}
}

bool c_text_editor::can_undo() const
{
	return !m_read_only && m_undo_index > 0;
}

bool c_text_editor::can_redo() const
{
	return !m_read_only && m_undo_index < (int)m_undo_buffer.size();
}

void c_text_editor::undo(int aSteps)
{
	while (can_undo() && aSteps-- > 0)
		m_undo_buffer[--m_undo_index].undo(this);
}

void c_text_editor::redo(int aSteps)
{
	while (can_redo() && aSteps-- > 0)
		m_undo_buffer[m_undo_index++].redo(this);
}

std::string c_text_editor::get_text() const
{
	return get_text(coordinates(), coordinates((int)m_lines.size(), 0));
}

std::vector<std::string> c_text_editor::get_text_lines() const
{
	std::vector<std::string> result;

	result.reserve(m_lines.size());

	for (auto& line : m_lines)
	{
		std::string text;

		text.resize(line.size());

		for (size_t i = 0; i < line.size(); ++i)
			text[i] = line[i].m_char;

		result.emplace_back(std::move(text));
	}

	return result;
}

std::string c_text_editor::get_selected_text() const
{
	return get_text(m_state.m_selection_start, m_state.m_selection_end);
}

std::string c_text_editor::get_current_line_text()const
{
	auto lineLength = get_line_max_column(m_state.m_cursor_position.m_line);
	return get_text(
		coordinates(m_state.m_cursor_position.m_line, 0),
		coordinates(m_state.m_cursor_position.m_line, lineLength));
}

void c_text_editor::proces_inputs()
{
}

void c_text_editor::colorize(int aFromLine, int aLines)
{
	int toLine = aLines == -1 ? (int)m_lines.size() : min((int)m_lines.size(), aFromLine + aLines);
	m_color_range_min = min(m_color_range_min, aFromLine);
	m_color_range_max = max(m_color_range_max, toLine);
	m_color_range_min = max(0, m_color_range_min);
	m_color_range_max = max(m_color_range_min, m_color_range_max);
	m_check_comments = true;
}

void c_text_editor::colorize_range(int aFromLine, int aToLine)
{
	if (m_lines.empty() || aFromLine >= aToLine)
		return;

	std::string buffer;
	std::cmatch results;
	std::string id;

	int endLine = max(0, min((int)m_lines.size(), aToLine));
	for (int i = aFromLine; i < endLine; ++i)
	{
		auto& line = m_lines[i];

		if (line.empty())
			continue;

		buffer.resize(line.size());
		for (size_t j = 0; j < line.size(); ++j)
		{
			auto& col = line[j];
			buffer[j] = col.m_char;
			col.m_color_idex = pallete_index::def;
		}

		const char* bufferBegin = &buffer.front();
		const char* bufferEnd = bufferBegin + buffer.size();

		auto last = bufferEnd;

		for (auto first = bufferBegin; first != last; )
		{
			const char* token_begin = nullptr;
			const char* token_end = nullptr;
			pallete_index token_color = pallete_index::def;

			bool hasTokenizeResult = false;

			if (m_language_definition.m_tokenize != nullptr)
			{
				if (m_language_definition.m_tokenize(first, last, token_begin, token_end, token_color))
					hasTokenizeResult = true;
			}

			if (hasTokenizeResult == false)
			{
				// todo : remove
				//printf("using regex for %.*s\n", first + 10 < last ? 10 : int(last - first), first);

				for (auto& p : m_regex_list)
				{
					if (std::regex_search(first, last, results, p.first, std::regex_constants::match_continuous))
					{
						hasTokenizeResult = true;

						auto& v = *results.begin();
						token_begin = v.first;
						token_end = v.second;
						token_color = p.second;
						break;
					}
				}
			}

			if (hasTokenizeResult == false)
			{
				first++;
			}
			else
			{
				const size_t token_length = token_end - token_begin;

				if (token_color == pallete_index::identifier)
				{
					id.assign(token_begin, token_end);

					// todo : allmost all language definitions use lower case to specify keywords, so shouldn't this use ::tolower ?
					if (!m_language_definition.m_case_sensitive)
						std::transform(id.begin(), id.end(), id.begin(), ::toupper);

					if (!line[first - bufferBegin].m_preprocessor)
					{
						if (m_language_definition.m_keywords.count(id) != 0)
							token_color = pallete_index::keyword;
						else if (m_language_definition.m_identifiers.count(id) != 0)
							token_color = pallete_index::known_identifier;
						else if (m_language_definition.m_preproc_identifiers.count(id) != 0)
							token_color = pallete_index::preproc_identifier;
					}
					else
					{
						if (m_language_definition.m_preproc_identifiers.count(id) != 0)
							token_color = pallete_index::preproc_identifier;
					}
				}

				for (size_t j = 0; j < token_length; ++j)
					line[(token_begin - bufferBegin) + j].m_color_idex = token_color;

				first = token_end;
			}
		}
	}
}

void c_text_editor::colorize_internal()
{
	if (m_lines.empty() || !m_colorizer_enabled)
		return;

	if (m_check_comments)
	{
		auto endLine = m_lines.size();
		auto endIndex = 0;
		auto commentStartLine = endLine;
		auto commentStartIndex = endIndex;
		auto withinString = false;
		auto withinSingleLineComment = false;
		auto withinPreproc = false;
		auto firstChar = true;			// there is no other non-whitespace characters in the line before
		auto concatenate = false;		// '\' on the very end of the line
		auto currentLine = 0;
		auto currentIndex = 0;
		while (currentLine < endLine || currentIndex < endIndex)
		{
			auto& line = m_lines[currentLine];

			if (currentIndex == 0 && !concatenate)
			{
				withinSingleLineComment = false;
				withinPreproc = false;
				firstChar = true;
			}

			concatenate = false;

			if (!line.empty())
			{
				auto& g = line[currentIndex];
				auto c = g.m_char;

				if (c != m_language_definition.m_preproc_char && !isspace(c))
					firstChar = false;

				if (currentIndex == (int)line.size() - 1 && line[line.size() - 1].m_char == '\\')
					concatenate = true;

				bool inComment = (commentStartLine < currentLine || (commentStartLine == currentLine && commentStartIndex <= currentIndex));

				if (withinString)
				{
					line[currentIndex].m_multiline_comment = inComment;

					if (c == '\"')
					{
						if (currentIndex + 1 < (int)line.size() && line[currentIndex + 1].m_char == '\"')
						{
							currentIndex += 1;
							if (currentIndex < (int)line.size())
								line[currentIndex].m_multiline_comment = inComment;
						}
						else
							withinString = false;
					}
					else if (c == '\\')
					{
						currentIndex += 1;
						if (currentIndex < (int)line.size())
							line[currentIndex].m_multiline_comment = inComment;
					}
				}
				else
				{
					if (firstChar && c == m_language_definition.m_preproc_char)
						withinPreproc = true;

					if (c == '\"')
					{
						withinString = true;
						line[currentIndex].m_multiline_comment = inComment;
					}
					else
					{
						auto pred = [](const char& a, const glyph& b) { return a == b.m_char; };
						auto from = line.begin() + currentIndex;
						auto& startStr = m_language_definition.m_comment_start;
						auto& singleStartStr = m_language_definition.m_single_line_comment;

						if (singleStartStr.size() > 0 &&
							currentIndex + singleStartStr.size() <= line.size() &&
							equals(singleStartStr.begin(), singleStartStr.end(), from, from + singleStartStr.size(), pred))
						{
							withinSingleLineComment = true;
						}
						else if (!withinSingleLineComment && currentIndex + startStr.size() <= line.size() &&
							equals(startStr.begin(), startStr.end(), from, from + startStr.size(), pred))
						{
							commentStartLine = currentLine;
							commentStartIndex = currentIndex;
						}

						inComment = inComment = (commentStartLine < currentLine || (commentStartLine == currentLine && commentStartIndex <= currentIndex));

						line[currentIndex].m_multiline_comment = inComment;
						line[currentIndex].m_comment = withinSingleLineComment;

						auto& endStr = m_language_definition.m_comment_end;
						if (currentIndex + 1 >= (int)endStr.size() &&
							equals(endStr.begin(), endStr.end(), from + 1 - endStr.size(), from + 1, pred))
						{
							commentStartIndex = endIndex;
							commentStartLine = endLine;
						}
					}
				}
				line[currentIndex].m_preprocessor = withinPreproc;
				currentIndex += UTF8CharLength(c);
				if (currentIndex >= (int)line.size())
				{
					currentIndex = 0;
					++currentLine;
				}
			}
			else
			{
				currentIndex = 0;
				++currentLine;
			}
		}
		m_check_comments = false;
	}

	if (m_color_range_min < m_color_range_max)
	{
		const int increment = (m_language_definition.m_tokenize == nullptr) ? 10 : 10000;
		const int to = min(m_color_range_min + increment, m_color_range_max);
		colorize_range(m_color_range_min, to);
		m_color_range_min = to;

		if (m_color_range_max == m_color_range_min)
		{
			m_color_range_min = INT_MAX;
			m_color_range_max = 0;
		}
		return;
	}
}

float c_text_editor::text_distance_to_line_start(const coordinates& aFrom) const
{
	auto& line = m_lines[aFrom.m_line];
	float distance = 0.0f;
	float spaceSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, " ", nullptr, nullptr).x;
	int colIndex = get_character_index(aFrom);
	for (size_t it = 0u; it < line.size() && it < colIndex; )
	{
		if (line[it].m_char == '\t')
		{
			distance = (1.0f + std::floor((1.0f + distance) / (float(m_tab_size) * spaceSize))) * (float(m_tab_size) * spaceSize);
			++it;
		}
		else
		{
			auto d = UTF8CharLength(line[it].m_char);
			char tempCString[7];
			int i = 0;
			for (; i < 6 && d-- > 0 && it < (int)line.size(); i++, it++)
				tempCString[i] = line[it].m_char;

			tempCString[i] = '\0';
			distance += ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, tempCString, nullptr, nullptr).x;
		}
	}

	return distance;
}

void c_text_editor::ensure_cursor_visible()
{
	if (!m_within_render)
	{
		m_scroll_to_cursor = true;
		return;
	}

	float scrollX = ImGui::GetScrollX();
	float scrollY = ImGui::GetScrollY();

	auto height = ImGui::GetWindowHeight();
	auto width = ImGui::GetWindowWidth();

	auto top = 1 + (int)ceil(scrollY / m_char_advance.y);
	auto bottom = (int)ceil((scrollY + height) / m_char_advance.y);

	auto left = (int)ceil(scrollX / m_char_advance.x);
	auto right = (int)ceil((scrollX + width) / m_char_advance.x);

	auto pos = get_actual_cursor_coordinates();
	auto len = text_distance_to_line_start(pos);

	if (pos.m_line < top)
		ImGui::SetScrollY(max(0.0f, (pos.m_line - 1) * m_char_advance.y));
	if (pos.m_line > bottom - 4)
		ImGui::SetScrollY(max(0.0f, (pos.m_line + 4) * m_char_advance.y - height));
	if (len + m_text_start < left + 4)
		ImGui::SetScrollX(max(0.0f, len + m_text_start - 4));
	if (len + m_text_start > right - 4)
		ImGui::SetScrollX(max(0.0f, len + m_text_start + 4 - width));
}

int c_text_editor::get_page_size() const
{
	auto height = ImGui::GetWindowHeight() - 20.0f;
	return (int)floor(height / m_char_advance.y);
}

c_text_editor::undo_record::undo_record(
	const std::string& aAdded,
	const c_text_editor::coordinates aAddedStart,
	const c_text_editor::coordinates aAddedEnd,
	const std::string& aRemoved,
	const c_text_editor::coordinates aRemovedStart,
	const c_text_editor::coordinates aRemovedEnd,
	c_text_editor::editor_state& aBefore,
	c_text_editor::editor_state& aAfter)
	: m_added(aAdded)
	, m_added_start(aAddedStart)
	, m_added_end(aAddedEnd)
	, m_removed(aRemoved)
	, m_removed_start(aRemovedStart)
	, m_removed_end(aRemovedEnd)
	, m_before(aBefore)
	, m_after(aAfter)
{
	assert(m_added_start <= m_added_end);
	assert(m_removed_start <= m_removed_end);
}

void c_text_editor::undo_record::undo(c_text_editor* aEditor)
{
	if (!m_added.empty())
	{
		aEditor->delete_range(m_added_start, m_added_end);
		aEditor->colorize(m_added_start.m_line - 1, m_added_end.m_line - m_added_start.m_line + 2);
	}

	if (!m_removed.empty())
	{
		auto start = m_removed_start;
		aEditor->insert_text_at(start, m_removed.c_str());
		aEditor->colorize(m_removed_start.m_line - 1, m_removed_end.m_line - m_removed_start.m_line + 2);
	}

	aEditor->m_state = m_before;
	aEditor->ensure_cursor_visible();

}

void c_text_editor::undo_record::redo(c_text_editor* aEditor)
{
	if (!m_removed.empty())
	{
		aEditor->delete_range(m_removed_start, m_removed_end);
		aEditor->colorize(m_removed_start.m_line - 1, m_removed_end.m_line - m_removed_start.m_line + 1);
	}

	if (!m_added.empty())
	{
		auto start = m_added_start;
		aEditor->insert_text_at(start, m_added.c_str());
		aEditor->colorize(m_added_start.m_line - 1, m_added_end.m_line - m_added_start.m_line + 1);
	}

	aEditor->m_state = m_after;
	aEditor->ensure_cursor_visible();
}