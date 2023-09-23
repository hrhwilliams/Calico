#ifndef _CALICO_OPENGL_RENDERER_HPP_
#define _CALICO_OPENGL_RENDERER_HPP_

#include <array>
#include <cstdint>
#include <exception>
#include <memory>
#include <set>
#include <string>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <vector>

#ifdef _DEBUG
#include <cstdio>
#include <iostream>
#endif // _DEBUG

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#endif // __APPLE__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using vao_t = uint32_t;
using vbo_t = uint32_t;
using ebo_t = uint32_t;
using uniform_t = uint32_t;
using program_t = uint32_t;
using shader_t = uint32_t;
using vertex_t = float;
using index_t = uint32_t;

#include "renderer/opengl/debug.hpp"
#include "renderer/opengl/uniform_buffer.hpp"
#include "renderer/opengl/shader.hpp"
#include "renderer/opengl/program.hpp"
#include "renderer/opengl/vertex_buffer.hpp"

#endif // _CALICO_OPENGL_RENDERER_HPP_