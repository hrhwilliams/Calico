#ifndef __CALICO_XML_PARSERS_HPP__
#define __CALICO_XML_PARSERS_HPP__

#include <string>

#include "asset/material.hpp"
#include "asset/program.hpp"
#include "logger/errors.hpp"

namespace Calico::XML {

class ParserError final : public Calico::Exception {
public:
    ParserError(const std::string &msg) : Calico::Exception(msg) {}
};

Calico::MaterialDescriptor read_material(const std::string &filename);
Calico::ProgramDescriptor read_program(const std::string &filename);

}

#endif // __CALICO_XML_PARSERS_HPP__
