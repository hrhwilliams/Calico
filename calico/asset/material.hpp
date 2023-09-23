#ifndef _CALICO_MATL_ASSET_
#define _CALICO_MATL_ASSET_

#include <stdexcept>
#include <string>
#include <unordered_map>

namespace Calico {

struct MaterialDescriptor {
    union Property {
        float f;
        bool b;
        float vec2[2];
        float vec3[3];
        float vec4[4];
        float mat2[4];
        float mat3[9];
        float mat4[16];
    };

    enum class PropertyType {
        Float,
        Bool,
        Vec2,
        Vec3,
        Vec4,
        Mat2,
        Mat3,
        Mat4,
    };

    std::string name = {};
    std::string shader = {};
    std::unordered_map<std::string, MaterialDescriptor::Property> properties = {};
    std::unordered_map<std::string, MaterialDescriptor::PropertyType> types = {};

    void add_property(const std::string &name, MaterialDescriptor::PropertyType type, MaterialDescriptor::Property &&property) {
        if (properties.contains(name) || types.contains(name)) {
            throw std::runtime_error("Attempting to insert property that already exists");
        }

        properties.insert({ name, std::move(property) });
        types.insert({ name, type });
    }
};

}

#endif // _CALICO_MATL_ASSET_