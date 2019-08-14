#pragma once

#pragma pack(push, 1)

static constexpr uint32_t PacketSignature = 0xDEADBEEF;

struct PacketHdr {
	uint32_t sig = PacketSignature;
	uint32_t size = {};

	auto valid() const noexcept {
		return sig == PacketSignature;
	}
};

#pragma pack(pop)
