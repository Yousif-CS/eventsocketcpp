#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <map>
#include <set>
#include <functional>
#include <deque>
#include <optional>
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <stdint.h>

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/detail/base64.hpp>

#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/reader.h>

namespace net = boost::asio;
namespace beast = boost::beast;
using namespace boost::beast;
using namespace boost::beast::websocket;