#ifndef __CALICO_GL_DEBUG_HPP__
#define __CALICO_GL_DEBUG_HPP__

namespace Calico::OpenGL {

#ifdef _DEBUG
const char *get_gl_error_string(std::size_t error_code) {
    switch (error_code) {
    case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    // case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
    // case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
    default: return "?";
    }
};

#define GLCALL(f) do {                                                      \
    int error;                                                              \
    f;                                                                      \
    while ((error = glGetError()) != 0) {                                   \
        std::printf("OpenGL call '%s:%d:%s' got error %d (%s)\n", __FILE__, \
                __LINE__, #f, error, get_gl_error_string(error));           \
    }                                                                       \
} while (0)
#else
#define GLCALL(f) f
#endif

}

#endif // __CALICO_GL_DEBUG_HPP__
