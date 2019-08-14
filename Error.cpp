#include "app.h"
	 
// App errors
struct AppErrorCategory : std::error_category {
const char* name() const noexcept override;
  std::string message(int ev) const override;
};
 
const char* AppErrorCategory::name() const noexcept {
	return "app";
}

std::string AppErrorCategory::message(int ev) const {
	auto ec = static_cast<Ec>(ev);
	switch (ec) {
		case Ec::SocketConnect:
			return "SocketConnect";
		case Ec::SocketRead:
			return "SocketRead";
		case Ec::SocketWrite:
			return "SocketWrite";
		case Ec::SocketSetup:
			return "SocketSetup";
		case Ec::InvalidHdr:
			return "InvalidHdr";
		case Ec::SocketAccept:
			return "SocketAccept";
		case Ec::SocketListen:
			return "SocketListen";
		case Ec::InvalidParam:
			return "InvalidParam";
		case Ec::SocketClosed:
			return "SocketClosed";
		default:
			return "Unknown";
	}
}
 
static const AppErrorCategory g_ecs{};

std::error_code make_error_code(Ec code) noexcept {
	return std::error_code{static_cast<int>(code), g_ecs};
}

// Errno errors
struct ErrnoErrorCategory : std::error_category {
	const char* name() const noexcept override;
	std::string message(int ev) const override;
};
 
const char* ErrnoErrorCategory::name() const noexcept {
	return "errno";
}

std::string ErrnoErrorCategory::message(int code) const {
	if (auto desc = strerror(code))
		return desc;
	return "N/A";
}
 
static const ErrnoErrorCategory g_errno{};

std::error_code make_error_code(int code) noexcept {
	return std::error_code{code, g_errno};
}
