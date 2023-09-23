#ifndef __CALICO_GL_UNIFORM_BUFFER_HPP__
#define __CALICO_GL_UNIFORM_BUFFER_HPP__

namespace Calico::OpenGL {

class UniformBuffer {
private:
    // delete and mark copy constructors as private
    UniformBuffer(const UniformBuffer &rhs) = delete;
    void operator=(const UniformBuffer &rhs) = delete;

    struct UniformBufferMemberTag {
        std::size_t size;
        std::size_t offset;
    };

    std::unordered_map<std::string, UniformBufferMemberTag> member_locations;

    static uint32_t ubo_count;
    std::string name = {};
    uint32_t ubo_id = 0;
    uint32_t binding_index = 0;
    uint32_t offset = 0;

    static std::size_t get_glsl_size(std::type_index type) {
        static const std::unordered_map<std::type_index, std::size_t> type_sizes = {
            { std::type_index(typeid(float)), 4 },
            { std::type_index(typeid(int32_t)), 4 },
            { std::type_index(typeid(bool)), 4 },
            { std::type_index(typeid(glm::vec3)), 12 },
            { std::type_index(typeid(glm::mat4)), 64 },
        };

        if (type_sizes.contains(type)) {
            return type_sizes.at(std::type_index(type));
        } else {
            throw std::runtime_error("Unrecognized type");
        }
    }
    // one name for each type
    // requires sizeof...(Types) == sizeof...(Names) and Names = const char* or std::string
    template <typename Type, typename... Types, typename Name, typename... Names>
        requires (sizeof...(Types) == sizeof...(Names))
    void calculate_offsets(Name name, Names... names) {
        auto type_size = get_glsl_size(std::type_index(typeid(Type)));
        member_locations.insert({ name, { type_size, offset } });
        offset += type_size;

        if constexpr (sizeof...(Types) > 0) {
            // set_contents<Types...>(std::forward<Names...>(names...));
            calculate_offsets<Types...>(names...);
        }
    }

    void create_buffer(std::size_t bytes) {
        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, ubo_id));
        GLCALL(glBufferData(GL_UNIFORM_BUFFER, bytes, nullptr, GL_STATIC_DRAW));
        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
        GLCALL(glBindBufferRange(GL_UNIFORM_BUFFER, binding_index, ubo_id, 0, bytes));
    }
public:
    // UniformBuffer() : name("") {}
    UniformBuffer(const std::string &name) : name(name) {
        binding_index = UniformBuffer::ubo_count++;
        GLCALL(glGenBuffers(1, &ubo_id));
    }

    UniformBuffer(UniformBuffer &&rhs) : name(rhs.name) {
        std::swap(this->ubo_id, rhs.ubo_id);
        std::swap(this->binding_index, rhs.binding_index);
        std::swap(this->offset, rhs.offset);
    }

    void operator=(UniformBuffer &&rhs) {
        this->name = std::move(rhs.name);
        std::swap(this->ubo_id, rhs.ubo_id);
        std::swap(this->binding_index, rhs.binding_index);
        std::swap(this->offset, rhs.offset);
    }

    inline const std::string &get_name() const {
        return name;
    }

    inline uint32_t get_binding_index() const {
        return binding_index;
    }

    template <typename... Types, typename... Names>
        requires (sizeof...(Types) == sizeof...(Names))
    void set_contents(Names... names) {
        calculate_offsets<Types...>(names...);
        create_buffer(offset);
    }

    template<typename T>
    void set_data(const std::string &name, const T &data) {
        // check if name in `member_locations` and that name does refer to a member of type `T`
        auto [size, var_offset] = member_locations[name];

        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, ubo_id));
        GLCALL(glBufferSubData(GL_UNIFORM_BUFFER, var_offset, size, &data));
        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    }
};

uint32_t UniformBuffer::ubo_count = 0;

}

#endif // __CALICO_GL_UNIFORM_BUFFER_HPP__
