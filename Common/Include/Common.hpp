#pragma once

#include <cstdint>

static constexpr auto IMCHAT_SERVER_IP = "127.0.0.1";
static constexpr uint16_t IMCHAT_SERVER_PORT = 9997;

static constexpr uint32_t MAX_TEXT_MESSAGE_LENGTH = 500;
static constexpr uint32_t MIN_USERNAME_LENGTH = 4;
static constexpr uint32_t MAX_USERNAME_LENGTH = 20;
static constexpr uint32_t MIN_PASSWORD_LENGTH = 5;
static constexpr uint32_t MAX_PASSWORD_LENGTH = 24;
static constexpr auto USERNAME_ALLOWED_CHARACTERS_STRING = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
static constexpr auto PASSWORD_ALLOWED_CHARACTERS_STRING = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_#$%&";

#define KiB(n) (n * 1024)
#define MiB(n) (n * 1024 * 1024)

#include "Message.hpp"
#include "Connection.hpp"
#include "TSQueue.hpp"
#include "Utility.hpp"
