#pragma once

namespace {

template<typename EntryType>
inline void splitField(const EntryType &entry, std::vector<std::string> &outVec) {
	outVec.emplace_back(toStr(entry));
}

template<typename ...Args>
inline auto splitFields(const Args & ... args) {
    std::vector<std::string> argVec;
    argVec.reserve(sizeof...(Args));

    auto processField = [&](const auto &entry) {
        splitField(entry, argVec);
    };

    (processField(args), ...);

    return argVec;
}

}   // namespace

// A type safe c++ way of doing printf through variadic template
template<typename ...Args>
inline auto format(std::string format, const Args &...args) {
    std::string result = format;

    if constexpr (sizeof...(Args) == 0)
        return result;

	// First break them apart into an array
	auto fieldArray = splitFields(args...);

	// Next, using the format string, replace every instance of %x with the
	// value from the matching field index
	for (int index = 0; index < fieldArray.size(); index++) {
		auto spec = toStr("%", index + 1);
		if (result.find(spec) != std::string::npos) {
			result = replace(spec, fieldArray[index], result);
			fieldArray[index].clear();
		}
	}

	// Append the rest
	for (auto &str : fieldArray) {
		if (str.empty())
			continue;
		result += " " + str;
	}

	// All done.
	return result;
}

inline auto consoleLock() {
	static std::mutex m;
	return std::unique_lock{m};
}

#define LOG( ...)	do { auto guard = consoleLock();  std::cout << std::hex << std::this_thread::get_id() << ":" << __FILE__ << ":" << std::dec << __LINE__ << " " << format(__VA_ARGS__) << std::endl; } while (false)

// Enable for extended debugging
#if DEBUG_LOG
	#define DLOG( ...)	do { auto guard = consoleLock();  std::cout << std::hex << std::this_thread::get_id() << ":" << __FILE__ << ":" << std::dec << __LINE__ << " " << format(__VA_ARGS__) << std::endl; } while (false)
#else
	#define DLOG( ...)	do { } while (false)
#endif
