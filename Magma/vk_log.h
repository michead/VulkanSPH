#pragma once

#include "spdlog.h"

typedef std::shared_ptr<spdlog::logger> Logger;

extern Logger logger;
