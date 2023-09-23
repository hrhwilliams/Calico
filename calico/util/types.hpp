#ifndef _TYPES_HPP_
#define _TYPES_HPP_

#include <cstdint>
#include <cmath>

using Entity = std::uint16_t;
using ComponentID = std::uint16_t;
using Index = std::uint16_t;

struct Vec3f {
    float x;
    float y;
    float z;

    inline Vec3f operator+(const Vec3f &rhs) const {
        return {
            .x = x + rhs.x,
            .y = y + rhs.y,
            .z = z + rhs.z
        };
    }

    inline Vec3f operator-(const Vec3f &rhs) const {
        return {
            .x = x - rhs.x,
            .y = y - rhs.y,
            .z = z - rhs.z
        };
    }

    inline float mag() const {
        return std::sqrt(x*x + y*y + z*z);
    }

    inline float dot(const Vec3f &rhs) const {
        return x*rhs.x + y*rhs.y + z*rhs.z;
    }

    inline Vec3f cross(const Vec3f &rhs) const {
        return {
            .x = (y * rhs.z - z * rhs.y),
            .y = -(x * rhs.z - z * rhs.x),
            .z = (x * rhs.y - y * rhs.x)
        };
    }
};

// TODO not working
class Mat4f {
    std::array<float, 16> elements;
public:
    float at(std::size_t row, std::size_t col) const {
        return elements[row * 4 + col];
    }

    float &at(std::size_t row, std::size_t col) {
        return elements[row * 4 + col];
    }

    Mat4f operator*(const Mat4f &rhs) const {
        Mat4f mat;

        for (auto row = 0u; row < 4u; row++) {
            for (auto col = 0u; col < 4u; col++) {
                float accumulator = 0.f;

                for (auto i = 0u; i < 4u; i++) {
                    accumulator += this->at(row, i) * rhs.at(i, col);
                }

                mat.at(row, col) = accumulator;
            }
        }

        return mat;
    }

    static Mat4f identity() {
        Mat4f mat;

        for (auto i = 0u; i < 4u; i++) {
            mat.at(i, i) = 1.0f;
        }

        return mat;
    }

    static Mat4f transform(const Vec3f &vec) {
        Mat4f mat = Mat4f::identity();
        mat.at(0, 3) = vec.x;
        mat.at(1, 3) = vec.y;
        mat.at(2, 3) = vec.z;
    }

    void print() const {
        for (auto i = 0u; i < 4u; i++) {
            std::printf("| %f, %f, %f, %f |\n",
                this->at(i, 0), this->at(i, 1), this->at(i, 2), this->at(i, 3));
        }
    }
};

#endif // _TYPES_HPP_
