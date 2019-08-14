#pragma once

// Convert any kinda arg to a string
template<char Delim, typename ...Msg>
inline auto toStrDelim(const Msg & ...msg) {
	std::stringstream stream;

	auto format = [&](auto &m, auto index) {
		if (index && Delim)
			stream.put(Delim);
		stream << m;
	};

	auto index = 0;
	(format(msg, index++), ...);

	return stream.str();
}

// Convert any kinda arg to a string space delimited 
template<typename ...Msg>
inline auto toStr(const Msg & ...msg) {
	return toStrDelim<'\0'>(msg...);
}

inline std::string replace(std::string_view key, std::string_view value, std::string haystack) noexcept {
    size_t pos = 0;
    while(true) {
        pos = haystack.find(key, pos);
        if (pos == std::string::npos)
            return haystack;

        haystack.replace(pos, key.size(), value);
        pos += value.size();
    }
}
