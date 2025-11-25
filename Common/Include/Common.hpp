#pragma once

#include <cstdint>
#include <vector>

typedef char byte_t;

#define IMCHAT_SERVER_IP "127.0.0.1"
#define IMCHAT_SERVER_PORT 9997

#define KB(n) (n * 1024)
#define MB(n) (n * 1024 * 1024)

#include "Message.hpp"
#include "Connection.hpp"
#include "TSQueue.hpp"
