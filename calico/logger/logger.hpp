#ifndef __CALICO_LOGGER_HPP__
#define __CALICO_LOGGER_HPP__

#include <cstdarg>
#include <cstdlib>
#include <string>

namespace Calico {

class Logger {
public:
    enum class Level {
        Debug = 0,
        Info = 1,
        Warning = 2,
        Error = 3,
        Fatal = 4,
    };
private:
    FILE *m_out = stdout;
    Logger::Level m_logging_level = Logger::Level::Debug;

public:
    static Logger& get();

    void set_logging_level(Logger::Level level) { m_logging_level = level; }
    void set_out_stdout() {
        if (!(m_out == stdout || m_out == stderr)) {
            std::fclose(m_out);
        }

        m_out = stdout;
    }

    void set_out_stderr() {
        if (!(m_out == stdout || m_out == stderr)) {
            std::fclose(m_out);
        }

        m_out = stderr;
    }

    void set_out_file(const std::string &filepath) {
        if (!(m_out == stdout || m_out == stderr)) {
            std::fclose(m_out);
        }

        m_out = std::fopen(filepath.c_str(), "a");
    }

    void log(Logger::Level level, const char *fmt...) const;
private:
    Logger() {}
    Logger(const Logger &rhs) = delete;
    Logger(Logger &&rhs) = delete;
    void operator=(const Logger &rhs) = delete;
    void operator=(Logger &&rhs) = delete;
    ~Logger() {
        if (!(m_out == stdout || m_out == stderr)) {
            std::fclose(m_out);
        }
    }

    static const char *level_to_string(Logger::Level level);
    static void _log_message(FILE *out, Logger::Level level, const char *fmt, va_list args);
};

}

#endif // __CALICO_LOGGER_HPP__
