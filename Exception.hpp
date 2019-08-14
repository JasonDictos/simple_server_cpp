#pragma once

class Exception : public std::exception {
public:
	template<typename Code, typename ...Msg>
	Exception(Code code, std::string file, size_t line, const Msg & ... msg) noexcept :
		m_file(std::move(file)), m_line(line), m_error(make_error_code(code)), m_message(format(msg...)) {
		m_message += " (" + m_error.message() + ") " + m_file + ":" + std::to_string(m_line);

		DLOG("Raising exception", what());
	}

	Exception(Exception &&) = default;
	Exception(const Exception &) = default;

	Exception & operator = (Exception &&) = default;
	Exception & operator = (const Exception &) = default;

	const char *what() const noexcept override {
		return m_message.c_str();
	}

private:
	std::string m_message;
	std::string m_file;
	size_t m_line = {};
	std::error_code m_error;
};

#define THROW(Ec, ...)	do { throw Exception(Ec, __FILE__, __LINE__, __VA_ARGS__); } while (false)
