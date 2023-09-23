#include <vector>
#include <cstring>
#include <memory>

#include "xml/parsers.hpp"
#include "logger/logger.hpp"

#include "thirdparty/tinyxml/tinyxml.h"

using namespace Calico;

// <?xml version="1.0" encoding="UTF-8"?>
// <material name="default_mat">
//     <shader name="default_shader"/>
//     <properties>
//         <property name="diffuse" type="vec3">0.0, 0.0, 0.0</property>
//         <property name="specular" type="vec3">0.0, 0.0, 0.0</property>
//         <property name="ambient" type="vec3">0.0, 0.0, 0.0</property>
//         <property name="shininess" type="float">32.0</property>
//     </properties>
// </material>

static std::vector<float> parse_float_array(const std::string &array_string, std::size_t elements) {
    std::vector<float> floats;
    floats.reserve(elements);

    for (std::size_t lhs_index = 0ull; lhs_index < array_string.size(); ) {
        std::size_t rhs_index = array_string.find(",", lhs_index);

        // last element in the list of floats
        if (rhs_index == std::string::npos) {
            rhs_index = array_string.size() - 1;
        }

        floats.push_back(std::stod(array_string.substr(lhs_index, rhs_index)));
        lhs_index = rhs_index + 1;
    }

    return floats;
}

static void copy_floats(const std::string &property_string, MaterialDescriptor::Property &property, std::size_t n) {
    auto floats = parse_float_array(property_string, n);
    if (floats.size() != n || floats.size() > sizeof(property.mat4) / sizeof(float)) {
        throw XML::ParserError("Error parsing float array in material descriptor");
    }

    // copying into the largest container of floats in property since it's just a union
    std::copy(floats.begin(), floats.begin() + n, property.mat4);
}

static std::optional<MaterialDescriptor::PropertyType> parse_property_type(const std::string &type) {
    if (type == "float") {
        return MaterialDescriptor::PropertyType::Float;
    } else if (type == "bool") {
        return MaterialDescriptor::PropertyType::Bool;
    } else if (type.substr(0, 3) == "vec" && type.size() == 4) {
        switch (type[3]) {
        case '2': return MaterialDescriptor::PropertyType::Vec2;
        case '3': return MaterialDescriptor::PropertyType::Vec3;
        case '4': return MaterialDescriptor::PropertyType::Vec4;
        default: return std::nullopt;
        }
    } else if (type.substr(0, 3) == "mat" && type.size() == 4) {
        switch (type[3]) {
        case '2': return MaterialDescriptor::PropertyType::Mat2;
        case '3': return MaterialDescriptor::PropertyType::Mat3;
        case '4': return MaterialDescriptor::PropertyType::Mat4;
        default: return std::nullopt;
        }
    }

    return std::nullopt;
}

static std::optional<MaterialDescriptor::Property> parse_material_property(
    std::optional<MaterialDescriptor::PropertyType> type, const std::string &property_string) {

    MaterialDescriptor::Property property;

    if (!type) {
        return std::nullopt;
    }

    switch (type.value()) {
    case MaterialDescriptor::PropertyType::Float: property.f = std::stod(property_string); break;
    case MaterialDescriptor::PropertyType::Bool: property.b = (property_string == "true" ? true : false); break;
    case MaterialDescriptor::PropertyType::Vec2: copy_floats(property_string, property, 2); break;
    case MaterialDescriptor::PropertyType::Vec3: copy_floats(property_string, property, 3); break;
    case MaterialDescriptor::PropertyType::Vec4: copy_floats(property_string, property, 4); break;
    case MaterialDescriptor::PropertyType::Mat2: copy_floats(property_string, property, 4); break;
    case MaterialDescriptor::PropertyType::Mat3: copy_floats(property_string, property, 9); break;
    case MaterialDescriptor::PropertyType::Mat4: copy_floats(property_string, property, 16); break;
    default: return std::nullopt;
    }

    return property;
}

MaterialDescriptor XML::read_material(const std::string &filename) {
    MaterialDescriptor material;
    TiXmlDocument matl_description(filename.c_str());

    if (matl_description.LoadFile()) {
        TiXmlHandle handle(&matl_description);
        TiXmlElement *element;
        TiXmlHandle root(0);

        element = handle.FirstChildElement().Element();
        root = TiXmlHandle(element);

        material.name = element->Attribute("name");

        element = root.FirstChild("shader").Element();
        material.shader = element->Attribute("name");

        element = root.FirstChild("properties").FirstChild().Element();
        for(; element != nullptr; element = element->NextSiblingElement()) {
            const char *name = element->Attribute("name");
            const char *type = element->Attribute("type");
            const char *data = element->GetText();

            auto property_type = parse_property_type(type);
            auto property = parse_material_property(property_type, data);
            if (property) {
                material.add_property(name, property_type.value(), std::move(property.value()));
            } else {
                auto logger = Logger::get();
                logger->log(Logger::Level::Warning, "Failed to add property");
            }
        }
    } else {
        throw XML::ParserError("Unable to open material descriptor '" + filename + "'");
    }

    return material;
}

// <?xml version="1.0" encoding="UTF-8"?>
// <program name="default_program" version="410 core">
//     <shaders>
//         <shader type="vertex" version="410 core">shaders/default.vert</shader>
//         <shader type="fragment" version="410 core">shaders/default.frag</shader>
//     </shaders>
//     <layout>
//         <location attrib="0" type="vec3"/>
//         <location attrib="1" type="vec3"/>
//     </layout>
//     <uniform-buffers>
//         <buffer>Globals</buffer>
//     </uniform-buffers>
//     <uniforms>
//         <uniform type="mat4">model</uniform>
//         <uniform type="float">scale</uniform>
//     </uniforms>
// </program>

ProgramDescriptor XML::read_program(const std::string &filename) {
    ProgramDescriptor program;
    TiXmlDocument program_description(filename.c_str());

    if (program_description.LoadFile()) {
        TiXmlHandle handle(&program_description);
        TiXmlElement *element;
        TiXmlHandle root(0);

        element = handle.FirstChildElement().Element();
        root = TiXmlHandle(element);

        program.name = element->Attribute("name");
        // TODO incomplete
    } else {
        throw XML::ParserError("Unable to open GLSL program descriptor '" + filename + "'");
    }

    return program;
}
