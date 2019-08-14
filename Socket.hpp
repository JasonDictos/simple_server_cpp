#pragma once

// This class implements the lowest level of the socket 
class Socket final {
public:
    static constexpr auto InvalidSocket = -1;

	Socket() = default;

	// Move ok
	Socket(Socket &&sock) :
   		m_hSocket(std::exchange(sock.m_hSocket, InvalidSocket)) {
	}

	Socket & operator = (Socket &&sock) {
		if (this == &sock)
			return *this;
		close();
   		m_hSocket = std::exchange(sock.m_hSocket, InvalidSocket);
		return *this;
	}

	// No copy
	Socket(const Socket &) = delete;
	Socket & operator = (const Socket &) = delete;

	~Socket() {
	   	close(); 
	}

	Socket(const std::string &address, uint64_t port) {
		connect(address, port);
	}

	void listen(const std::string &address, uint16_t port) {
		close();

		DLOG("Listen");

		m_hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
		if (m_hSocket < 0)
			THROW(errno, "Failed to create listen socket");

		struct sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		inet_pton(AF_INET, address.c_str(), &addr.sin_addr);

		auto res = ::bind(m_hSocket, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr));
		if (res < 0)
			THROW(errno, "Failed to bind listen socket");

		res = ::listen(m_hSocket, 5);
		if (res < 0)
			THROW(errno, "Failed to listen");
	}

	Socket accept() {
		DLOG("Accept");

		struct sockaddr addr;
		socklen_t len = sizeof(addr);
		auto hNewSock = ::accept(m_hSocket, &addr, &len);
		if (hNewSock < 0)
			THROW(errno, "Failed to accept");

		Socket newSocket;
		newSocket.m_hSocket = hNewSock;
		return newSocket;
	}

	// Send data on a socket 
	void write(const uint8_t *data, size_t size) {
		if (size == 0)
			THROW(Ec::InvalidParam, "Write of 0");

		DLOG("Write %1 bytes", size);

		size_t len = 0;
		while (len < size) {
			DLOG("Write", size);

			// Send it
			auto sentSize = ::send(m_hSocket, data + len, static_cast<int>(size - len), 0);

			// If it had an error, stop
			if (sentSize < 0)
				THROW(errno, "Failed to write %1 bytes", size - len);

			len += sentSize;
		}

		return;
	}

	// Recv data form a socket 
	size_t read(uint8_t *data, size_t size) {
		if (size == 0)
			THROW(Ec::InvalidParam, "Read of 0");

		DLOG("Read %1 bytes", size);

		size_t len = 0;
		while (len < size) {
			// Receive as much as we can
			auto recvSize = ::recv(m_hSocket, data + len, static_cast<int>(size - len), 0);
			if (recvSize < 0)
				THROW(errno, "Failed to write %1 bytes", size - len);

			if (recvSize == 0)
				THROW(Ec::SocketClosed, "Socket closed on read");

			DLOG("Read %1/%2 (%3)", len, size, recvSize);

			len += recvSize;
		}
		return len;
	}

	// Open a socket to the given address
	void connect(const std::string &address, uint16_t port) {
		DLOG("Connect %1:%2", address, port);

        // Setup the getaddrinfo params
        struct addrinfo
            *rp = NULL,
            hints;	
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        // Resolve the server address and port
        struct addrinfo *result = nullptr;
        int socketStatus;
        if (socketStatus = ::getaddrinfo(address.c_str(), std::to_string(port).c_str(), &hints, &result))
            THROW(Ec::SocketSetup, "Failed to getaddrinfo");

        auto releaseGuard = Guard{[&]() { ::freeaddrinfo(result); }};

        // getaddrinfo returns a list of address structures.
        // Try each address until we successfully connect.
        // If socket call fails, close the socket and try the
        // next address.
        for (rp = result; rp != NULL; rp = rp->ai_next)	{
            auto sfd = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (sfd == -1)
                continue;

            auto closeGuard = Guard{[&]() { ::close(sfd); }};

            // Set a long timeout
            // tv_sec is number of whole seconds of elapsed time
            struct timeval timeout;
            timeout.tv_sec = 60 * 5;
            timeout.tv_usec = 0;

            int iResult;

            // Set send
            iResult = ::setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout, sizeof(timeout));
            if (iResult) 
				THROW(Ec::SocketSetup, "Failed to set SO_RCVTIMEO");

            // Set recv
            iResult = ::setsockopt(sfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
            if (iResult)
				THROW(Ec::SocketSetup, "Failed to set SO_SNDTIMEO");

            // check the options were set correctly - recieve
            struct timeval timeoutGetRec;
            struct timeval timeoutGetSend;
            {
                socklen_t optLen = sizeof(timeoutGetRec);
                iResult = ::getsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO ,
                    (char *)&timeoutGetRec, &optLen);

                if (iResult) 
					THROW(Ec::SocketSetup, "Failed to set SO_RCVTIMEO");
            }

            // check the options were set correctly - send
            {
                socklen_t optLen = sizeof(timeoutGetSend);
                iResult = ::getsockopt(sfd, SOL_SOCKET, SO_SNDTIMEO,
                    (char *)&timeoutGetSend, &optLen);

                if (iResult)
					THROW(Ec::SocketSetup, "Failed to set SO_SNDTIMEO");
            }
            // Verify timeouts are what we want them to be
            if (timeoutGetRec.tv_sec != timeout.tv_sec || timeoutGetSend.tv_sec != timeout.tv_sec)
				THROW(Ec::SocketSetup, "Failed to set SO_SNDTIMEO");

            // After sockets option are defined establish connection
            if (::connect(sfd, rp->ai_addr, rp->ai_addrlen))
                continue;

            m_hSocket = sfd;
            closeGuard.cancel();
			return;
        }

		THROW(errno, "Failed to connect to %1:%2", address, port);
    }

	// Close a socket 
	void close() {
		DLOG("Close");

        // If the socket is not open, done
        if (m_hSocket != InvalidSocket) {
            ::shutdown(m_hSocket, SHUT_RDWR);
            ::close(m_hSocket);
            m_hSocket = InvalidSocket;
        }
	}

private:
	// Holds the socket handle 
	int m_hSocket = InvalidSocket;
};
