#pragma once

#define IMCHAT_SERVER_IP "127.0.0.1"
#define IMCHAT_SERVER_PORT 9997

static constexpr auto MAX_TEXT_MESSAGE_LENGTH = 500u;
static constexpr auto MAX_USERNAME_LENGTH = 20u;
static constexpr auto MAX_PASSWORD_LENGTH = 24u;
static constexpr auto USERNAME_ALLOWED_CHARACTERS_STRING = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
static constexpr auto PASSWORD_ALLOWED_CHARACTERS_STRING = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_#$%&";

#define KB(n) (n * 1024)
#define MB(n) (n * 1024 * 1024)

#include "Message.hpp"
#include "Connection.hpp"
#include "TSQueue.hpp"
#include "Utility.hpp"
