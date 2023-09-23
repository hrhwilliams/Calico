#ifndef __CALICO_GL_VERTEX_BUFFER_HPP__
#define __CALICO_GL_VERTEX_BUFFER_HPP__

namespace Calico::OpenGL {

class IVertexArray {
public:
    virtual ~IVertexArray() = default;
    virtual void draw(const Program &) = 0;
};

template <typename... Types>
class VertexArray final : public IVertexArray {
    vao_t vao_id = 0;
    vbo_t vbo_id = 0;
    ebo_t ebo_id = 0;

    std::size_t vertex_count = 0;
    std::size_t offset = 0;
    std::size_t attrib_index = 0;

    static std::size_t glsl_get_elements_per_type(std::type_index type) {
        static const std::unordered_map<std::type_index, std::size_t> type_to_elements = {
            { std::type_index(typeid(float)), 1 },
            { std::type_index(typeid(glm::vec2)), 2 },
            { std::type_index(typeid(glm::vec3)), 3 },
            { std::type_index(typeid(glm::vec4)), 4 },
        };

        if (type_to_elements.contains(type)) {
            return type_to_elements.at(type);
        } else {
            throw std::runtime_error("Unrecognized type for attrib");
        }
    }

    template <typename Array, typename... Arrays>
    std::size_t _calculate_total_array_size(Array array, Arrays... arrays) {
        std::vector<float> data = array;
        std::size_t bytes = data.size() * sizeof(float);

        if constexpr (sizeof...(Arrays) > 0) {
            bytes += _calculate_total_array_size(arrays...);
        }

        return bytes;
    }

    template <typename Type, typename... _Types, typename Array, typename... Arrays>
    void _set_data(Array array, Arrays... arrays) {
        std::vector<float> data = array;
        std::size_t elements_per_type = glsl_get_elements_per_type(std::type_index(typeid(Type)));

        GLCALL(glBufferSubData(GL_ARRAY_BUFFER, offset, data.size() * sizeof(float), &data[0]));
        GLCALL(glVertexAttribPointer(attrib_index, elements_per_type, GL_FLOAT, GL_FALSE,
                elements_per_type * sizeof(float), (void*) offset));
        GLCALL(glEnableVertexAttribArray(attrib_index++));

        offset += data.size() * sizeof(float);

        if constexpr (sizeof...(Arrays) > 0) {
            _set_data<_Types...>(arrays...);
        }
    }
public:
    VertexArray() {
        glGenVertexArrays(1, &vao_id);
        glGenBuffers(1, &vbo_id);
        glGenBuffers(1, &ebo_id);
    }

    VertexArray(const VertexArray &rhs) = delete;
    VertexArray(VertexArray &&rhs) {
        std::swap(this->vao_id, rhs.vao_id);
        std::swap(this->vbo_id, rhs.vbo_id);
        std::swap(this->ebo_id, rhs.ebo_id);
    }

    void operator=(const VertexArray &rhs) = delete;
    void operator=(VertexArray &&rhs) = delete;

    ~VertexArray() {
        glDeleteVertexArrays(1, &vao_id);
        glDeleteBuffers(1, &vbo_id);
        glDeleteBuffers(1, &ebo_id);
    }

    VertexArray<Types...> &set_indices(const std::vector<unsigned int> &indices) {
        glBindVertexArray(vao_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);

        GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                &indices[0], GL_STATIC_DRAW));
        this->vertex_count = indices.size();

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return *this;
    }

    template <typename... Arrays>
        requires (sizeof...(Types) == sizeof...(Arrays))
    VertexArray<Types...> &set_data(Arrays... arrays) {
        glBindVertexArray(vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_id);

        std::size_t buffer_size_bytes = _calculate_total_array_size(arrays...);
        glBufferData(GL_ARRAY_BUFFER, buffer_size_bytes, nullptr, GL_STATIC_DRAW);
        _set_data<Types...>(arrays...);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return *this;
    }

    virtual void draw(const Program &program) override {
        GLCALL(glUseProgram(program));
        GLCALL(glBindVertexArray(vao_id));
        GLCALL(glDrawElements(GL_TRIANGLES, this->vertex_count, GL_UNSIGNED_INT, 0));
        GLCALL(glBindVertexArray(0));
    }
};

}

#endif // __CALICO_GL_VERTEX_BUFFER_HPP__
