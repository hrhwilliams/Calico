#ifndef _CALICO_SHADER_ASSET_
#define _CALICO_SHADER_ASSET_

#include <string>
#include <unordered_map>
#include <vector>

namespace Calico {

struct ProgramDescriptor {
    enum class ShaderType {
        Vertex,
        Fragment,
        Geometry,
    };

    std::string name;
    std::unordered_map<std::string, std::string> shader_to_source;
    std::unordered_map<std::string, ShaderType> shader_to_type;
    std::unordered_map<std::string, std::string> uniforms;
    std::vector<std::string> uniform_buffers;
};

}

#endif // _CALICO_SHADER_ASSET_