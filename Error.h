#pragma once

enum class Ec {
	SocketConnect,
	SocketRead,
	SocketWrite,
	SocketSetup,
	SocketListen,
	SocketAccept,
	SocketClosed,
	InvalidHdr,
	InvalidParam,
};

namespace std {

// Specialize the is_error_code_enum template to resolve to true for our error
// codes
template<>
struct is_error_code_enum<Ec> : true_type {};

}	// namespace std

std::error_code make_error_code(Ec code) noexcept;
std::error_code make_error_code(int code) noexcept;
