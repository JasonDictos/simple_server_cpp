#pragma once
#pragma once

// List of signals to intercept not including every single one of them
static constexpr std::array<int, 10> SigList = {
	// Termination request signal
	SIGTERM,

	// Critical signals
	SIGFPE, SIGILL, SIGSEGV, SIGBUS, SIGABRT, SIGPIPE,

	// Non critical signals
	SIGINT, SIGQUIT, SIGHUP
};

// De-registers the signal handler
inline void disableSignalHandler() noexcept {
	std::cerr << "De-registering handlers" << std::endl;
	for (auto sig : SigList)
		::signal(sig, SIG_DFL);
}

// Signal handler for Linux.
inline void signalHandler(int sig) noexcept {
	// Hups are allowed, ignore for now
	if (sig == SIGHUP)
		return;

	// Check graceful request to terminate first
	if (sig == SIGTERM || sig == SIGQUIT) {
		cancelFlag() = true;
		return;
	}

	// From here on out, we're exiting no matter what. De-register our signal
	// handler so we really exit if we're in a abort loop
	disableSignalHandler();

	// Handle first interruption as a cancellation request second will exit hard
	if (sig == SIGINT) {
		cancelFlag() = true;
		return;
	}

	LOG("Dying due to signal %1", sig);

	::exit(EXIT_FAILURE);
}

// Setup all the signal handlers
inline void setupSignalHandler() noexcept {
	// Setup sigaction object
	struct ::sigaction sigact;
	sigact.sa_handler = signalHandler;
	sigact.sa_flags = 0;

	// Register handler for all signals we are intercepting
	sigemptyset(&sigact.sa_mask);
	for (auto sig : SigList) {
		::sigaddset(&sigact.sa_mask, sig);
		::sigaction(sig, &sigact, nullptr);
	}
}
