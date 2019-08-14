#pragma once

inline auto & cancelFlag() {
	static std::atomic<bool> flag = {};
	return flag;
}

inline auto cancelled() {
	return cancelFlag() == true;
}
