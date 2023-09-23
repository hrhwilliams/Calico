#ifndef _CALICO_GL_SHADER_HPP_
#define _CALICO_GL_SHADER_HPP_

namespace Calico::OpenGL {

class Shader {
    shader_t shader_id = 0;

    inline std::string get_info_log() const noexcept {
        if (!glIsShader(shader_id)) {
            return {};
        }

        int32_t info_log_length;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

        if (info_log_length > 0) {
            std::unique_ptr<char[]> info_log(new char[info_log_length]);
            glGetShaderInfoLog(shader_id, info_log_length, nullptr, info_log.get());
            return std::string(info_log.get());
        } else {
            return {};
        }   
    }

public:
    Shader(const char *source, uint32_t shader_type) {
        shader_id = glCreateShader(shader_type);
        glShaderSource(shader_id, 1, &source, nullptr);
        glCompileShader(shader_id);

        // check for shader compile errors
        int success;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
        if (!success) {
            throw std::runtime_error(get_info_log());
        }
    }

    Shader(const Shader &rhs) = delete;
    void operator=(const Shader &rhs) = delete;

    Shader(Shader &&rhs) {
        std::swap(this->shader_id, rhs.shader_id);
    }

    void operator=(Shader &&rhs) {
        std::swap(this->shader_id, rhs.shader_id);
    }

    ~Shader() {
        if (shader_id > 0) {
            glDeleteShader(shader_id);
        }
    }

    operator shader_t() const {
        return this->shader_id;
    }
};

}

#endif // _CALICO_GL_SHADER_HPP_
