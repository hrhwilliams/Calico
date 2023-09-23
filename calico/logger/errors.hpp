#ifndef __CALICO_ERRORS_HPP__
#define __CALICO_ERRORS_HPP__

#include <stdexcept>
#include <string>

namespace Calico {

class Exception : public std::runtime_error {
    std::string m_msg = {};
public:
    Exception(const std::string &msg);
    virtual const char* what() const noexcept;
};

class RuntimeError final : public Exception {
public:
    RuntimeError(const std::string &msg) : Exception(msg) {}
};

}

#endif // __CALICO_ERRORS_HPP__