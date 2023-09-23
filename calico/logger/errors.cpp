#include "errors.hpp"

using namespace Calico;

#ifdef _CALICO_BACKTRACE_ON_ERROR
#include <cstdlib>
#include <sstream>

#include <execinfo.h>

static inline char** get_backtrace(std::size_t max_frames, std::size_t *returned_frames) {
    void *backtrace_frames[max_frames];
    *returned_frames = backtrace(&backtrace_frames[0], max_frames);
    return backtrace_symbols(backtrace_frames, *returned_frames);
}
#endif // _CALICO_BACKTRACE_ON_ERROR

Exception::Exception(const std::string &msg): std::runtime_error(msg.c_str()) {
#ifdef _CALICO_BACKTRACE_ON_ERROR
    std::stringstream ss;
    ss << msg << "\n[[--- Traceback ---]]\n";

    std::size_t frames;
    char **callstack = get_backtrace(64, &frames);

    for (auto i = 1u; i < frames; i++) {
        ss << callstack[i] << '\n';
    }

    std::free(callstack);
    m_msg = ss.str();
#else
    m_msg = std::string(msg);
#endif // _CALICO_BACKTRACE_ON_ERROR
}

const char* Exception::what() const noexcept {
    return m_msg.c_str();
}