#include "app.h"

static constexpr auto ListenAddr = "0.0.0.0";
static constexpr uint16_t ListenPort = 5555;

void onNewClient(Socket sock) {
	try {

		LOG("New client");

		Connection client(std::move(sock));

		while (!cancelled()) {
			auto msg = client.read();
			LOG("Server received client message %1", msg);

			client.write("Got the message!"s);
		}
	} catch (const std::exception &e) {
		if (cancelled())
			return;
		LOG("Server thread errored", e.what());
	}
}

int main() {
	// Cancel the app/server when we exit this frame
	auto cancelGuard = Guard{[&]{ cancelFlag() = true; }};

	// Listen for signals so we can exit
	setupSignalHandler();

	// Start a server listening on port 555
	Server server(ListenAddr, ListenPort, std::bind(onNewClient, _1));

	// Give the server a second to listen
	std::this_thread::sleep_for(50ms);

	// Connect a client, send a hello
	Connection conn("127.0.0.1", ListenPort);

	conn.write("Hi there!"s);

	// See if we can get a response
	auto res = conn.read();

	LOG("Client received server message %1", res);

	// Stop the show
	cancelFlag() = true;
	return 0;
}
