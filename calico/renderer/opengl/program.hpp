#ifndef _CALICO_GL_PROGRAM_HPP_
#define _CALICO_GL_PROGRAM_HPP_

namespace Calico::OpenGL {

class Program {
    std::unordered_map<const char *, uniform_t> uniforms = {};
    program_t program_id = 0;
    bool linked = false;
    bool vertex_shader_attached = false;
    bool fragment_shader_attached = false;

    inline std::string get_info_log() const noexcept {
        if (!glIsProgram(program_id)) {
            return {};
        }

        int32_t info_log_length;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);

        if (info_log_length > 0) {
            std::unique_ptr<char[]> info_log(new char[info_log_length]);
            glGetProgramInfoLog(program_id, info_log_length, nullptr, info_log.get());
            return std::string(info_log.get());
        } else {
            return {};
        }   
    }

    void attach(const Shader &shader) {
        int32_t shader_type;
        glGetShaderiv(shader, GL_SHADER_TYPE, &shader_type);

        // check if shader already assigned
        if (shader_type == GL_VERTEX_SHADER) {
            if (!vertex_shader_attached) {
                vertex_shader_attached = true;
            } else {
                throw std::runtime_error("Attempting to attach vertex shader to program that already has one attached");
            }
        } else if (shader_type == GL_FRAGMENT_SHADER) {
            if (!fragment_shader_attached) {
                fragment_shader_attached = true;
            } else {
                throw std::runtime_error("Attempting to attach fragment shader to program that already has one attached");
            }
        }

        glAttachShader(program_id, shader);
    }

    std::optional<uniform_t> get_uniform(const char *name) {
        if (!uniforms.contains(name)) {
            uniform_t uniform_id = glGetUniformLocation(program_id, name);
            if (uniform_id == -1) {
                return std::nullopt;
            } else {
                uniforms.insert({ name, uniform_id });
            }
        }

        return uniforms.at(name);
    }
public:
    Program() {
        program_id = glCreateProgram();
    }

    Program(const Program &rhs) = delete;
    void operator=(const Program &rhs) = delete;

    Program(Program &&rhs) {
        std::swap(this->program_id, rhs.program_id);
    }

    void operator=(Program &&rhs) {
        std::swap(this->program_id, rhs.program_id);
    }

    // It's not obvious if failing to set a uniform is actually an error
    // e.g. when setting global uniforms across all shaders, it isn't an
    // error for a shader to not use a specific global. Therefore this
    // returns a bool to denote success or failure which allows the program
    // to then decide if this is an error or not.
    bool set_uniform_mat4(const char *name, glm::mat4 matrix) {
        GLCALL(glUseProgram(program_id));

        auto uniform_id = get_uniform(name);
        if (uniform_id) {
            glUniformMatrix4fv(uniform_id.value(), 1, GL_FALSE, glm::value_ptr(matrix));
        } else {
            GLCALL(glUseProgram(0));
            return false;
        }

        GLCALL(glUseProgram(0));
        return true;
    }

    bool set_uniform_float(const char *name, float x) {
        GLCALL(glUseProgram(program_id));

        auto uniform_id = get_uniform(name);
        if (uniform_id) {
            glUniform1f(uniform_id.value(), x);
        } else {
            GLCALL(glUseProgram(0));
            return false;
        }

        GLCALL(glUseProgram(0));
        return true;
    }

    Program &attach_vertex_shader(const char *source) {
        attach(Shader(source, GL_VERTEX_SHADER));
        return *this;
    }

    Program &attach_fragment_shader(const char *source) {
        attach(Shader(source, GL_FRAGMENT_SHADER));
        return *this;
    }

    void link() {
        if (!(vertex_shader_attached && fragment_shader_attached)) {
            // error: need vert and frag shader before linking
            throw std::runtime_error("Program requires both fragment shader and vertex shader to be attached before linking");
        }

        glLinkProgram(program_id);
        // check for linking errors
        int success;
        glGetProgramiv(program_id, GL_LINK_STATUS, &success);
        if (!success) {
            throw std::runtime_error(get_info_log());
        } else {
            linked = true;
        }
    }

    void attach_uniform_buffer(const UniformBuffer &ubo) const {
        int32_t block_index = glGetUniformBlockIndex(program_id, ubo.get_name().c_str());
        GLCALL(glUniformBlockBinding(program_id, block_index, ubo.get_binding_index()));
    }

    ~Program() {
        if (program_id > 0) {
            glDeleteProgram(program_id);
        }
    }

    operator program_t() const {
        return this->program_id;
    }
};

}

#endif // _CALICO_GL_PROGRAM_HPP_
