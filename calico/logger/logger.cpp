#include "logger.hpp"

#include <ctime>
#include <cstdio>

using namespace Calico;

Logger& Logger::get() {
    static Logger logger;
    return logger;
}

void Logger::log(Logger::Level level, const char *fmt...) const {
    if (level < m_logging_level) {
        return;
    }

    va_list args;
    va_start(args, fmt);

    _log_message(m_out, level, fmt, args);

    va_end(args);
}

const char* Logger::level_to_string(Logger::Level level) {
    switch (static_cast<std::size_t>(level)) {
    case 0: return "Debug";
    case 1: return "Info";
    case 2: return "Warning";
    case 3: return "Error";
    case 4: return "Severe";
    default: return "Unknown";
    }
}

void Logger::_log_message(FILE *out, Logger::Level level, const char *fmt, va_list args) {
    char time_buffer[20];

    std::time_t t = std::time(nullptr);
    std::strftime(time_buffer, 20, "%F %T", std::localtime(&t));
    std::fprintf(out, "[%s] ", level_to_string(level));
    std::fprintf(out, "[%s] ", time_buffer);
    std::vfprintf(out, fmt, args);
    std::fprintf(out, "\n");
}
