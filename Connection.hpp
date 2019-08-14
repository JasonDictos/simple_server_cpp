#pragma once

class Connection {
public:
	Connection(Socket &&socket) :
		m_socket(std::move(socket)) {
	}

	Connection(std::string addr, uint16_t port) :
		m_socket(std::move(addr), port) {
	}

	auto write(const std::string &packet) {
		// First the header, then the packet
		PacketHdr hdr{PacketSignature, static_cast<uint32_t>(packet.size())};
		m_socket.write(reinterpret_cast<const uint8_t *>(&hdr), sizeof(hdr));

		// Now send the packet
		m_socket.write(reinterpret_cast<const uint8_t *>(packet.data()), packet.size());
	}

	auto read() {
		// First read the header
		auto hdr = recvHdr();
		
		// Next read the data into the container
		std::string res;
		res.resize(hdr.size);

		m_socket.read(reinterpret_cast<uint8_t *>(res.data()), res.size());

		return res;
	}

private:
	PacketHdr recvHdr() {
		PacketHdr packet;

		m_socket.read(reinterpret_cast<uint8_t *>(&packet), sizeof(packet));

		if (!packet.valid())
			THROW(Ec::InvalidHdr, "Invalid packet signature %1 != %2", packet.sig, PacketSignature);

		return packet;
	}

	auto recvPayload(size_t len) {
		std::string res;
		res.resize(len);

		m_socket.read(reinterpret_cast<uint8_t *>(res.data()), res.size());

		return res;
	}

	Socket m_socket;
};
