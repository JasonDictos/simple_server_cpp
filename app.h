#pragma once
#include <stdio.h>
#include <vector>
#include <signal.h>
#include <arpa/inet.h>
#include <atomic>
#include <set>
#include <iostream>
#include <sstream>
#include <tuple>
#include <string_view>
#include <algorithm>
#include <array>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <system_error>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netdb.h>
#include <functional>
#include <unistd.h>

using namespace std::literals::string_literals;
using namespace std::placeholders;
using namespace std::literals::chrono_literals;

#include "traits.hpp"
#include "string.hpp"
#include "format.hpp"
#include "cancel.hpp"
#include "signal.hpp"
#include "Error.h"
#include "Exception.hpp"
#include "Guard.hpp"
#include "Socket.hpp"
#include "Packet.hpp"
#include "Connection.hpp"
#include "Server.hpp"
