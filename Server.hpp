#pragma once

class Server {
public:
	using Callback = std::function<void(Socket)>;

	Server(std::string addr, uint16_t port, Callback &&packetCallback) :
		m_addr(std::move(addr)), m_port(port), m_callback(std::move(packetCallback)),
		m_thread{std::bind(&Server::serverLoop, this)} {

		DLOG("Server starting %1:%2", m_addr, port);
	}

	~Server() {
		m_socket.close();
		m_thread.join();
	}

private:
	void serverLoop() {
		try {
			auto logGuard = Guard{std::bind(&Server::serverLoopStart, this), std::bind(&Server::serverLoopStop, this)};

			m_socket.listen("0.0.0.0", m_port);

			// Until we are asked to stop
			while (!cancelled()) {
				DLOG("Accepting %1:%2", m_addr, m_port);

				auto newSocket = m_socket.accept();

				DLOG("Accepted new socket");

				// Spawn a new thread and hand it off to the callback
				m_clients.emplace_back(m_callback, std::move(newSocket));
			}
		} catch (...) {
		}
	}

	void serverLoopStart() {
		DLOG("Server starting listening on address %1:%2", m_addr, m_port);
	}

	void serverLoopStop() {
		DLOG("Server stopping");

		// Ensure all clients are joined on
		for (auto &client : m_clients)
			client.join();

		m_clients.clear();
	}

	Callback m_callback;
	Socket m_socket;
	std::string m_addr;
	uint16_t m_port;
	std::vector<std::thread> m_clients;

	std::thread m_thread;
};
