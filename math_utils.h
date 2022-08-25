#pragma once
#include "config.h"

//DEPTH [0,1] Vulakn / DirectX like//
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RIGHT_HANDED 
#define GLM_FORCE_RADIANS

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/matrix_access.hpp> 

#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/ext.hpp>

namespace raster3d
{
    using  vec2 = glm::vec2;
    using dvec2 = glm::dvec2;
    using ivec2 = glm::ivec2;

    using  vec3 = glm::vec3;
    using dvec3 = glm::dvec3;
    using ivec3 = glm::ivec3;

    using  vec4 = glm::vec4;
    using dvec4 = glm::dvec4;
    using ivec4 = glm::ivec4;

    using  mat3 = glm::mat3;
    using dmat3 = glm::dmat3;

    using  mat4 = glm::mat4;
    using dmat4 = glm::dmat4;

    using quat = glm::quat;
    using dquat = glm::dquat;

    template < typename scalar, glm::precision P = glm::defaultp >  using  tvec2 = glm::tvec2< scalar, P >;
    template < typename scalar, glm::precision P = glm::defaultp >  using  tvec3 = glm::tvec3< scalar, P >;
    template < typename scalar, glm::precision P = glm::defaultp >  using  tvec4 = glm::tvec4< scalar, P >;
    template < typename scalar, glm::precision P = glm::defaultp >  using  tquat = glm::tquat< scalar, P >;
    template < typename scalar, glm::precision P = glm::defaultp >  using  tmat3 = glm::tmat3x3< scalar, P >;
    template < typename scalar, glm::precision P = glm::defaultp >  using  tmat4 = glm::tmat4x4< scalar, P >;

    namespace constants
    {
        template <class T>
        inline T pi()
        {
            return glm::pi<T>();
        }
        template <class T>
        inline T pi2()
        {
            return glm::pi<T>() * ((T)2.0);
        }
        template <class T>
        inline T e()
        {
            return glm::e<T>();
        }
        template <class T>
        inline T golden_ratio()
        {
            return glm::golden_ratio<T>();
        }
        template <class T>
        inline const T& identity()
        {
            static T I(1.0);
            return I;
        }
    }


    template < class T >
    T max(const T& a, const T& b)
    {
        return b < a ? a : b;
    }

    template < class T, typename ...ARGS>
    T max(const T& a, const T& b, ARGS... args)
    {
        return max(max<T>(a, b), args...);
    }

    template < class T >
    T min(const T& a, const T& b)
    {
        return a < b ? a : b;
    }

    template < class T, typename ...ARGS>
    T min(const T& a, const T& b, ARGS... args)
    {
        return min(min<T>(a, b), args...);
    }

    template < class T >
    inline T dot(const T& a, const T& b)
    {
        return glm::dot(a, b);
    }

    template < class T >
    inline T cross(const T& a, const T& b)
    {
        return glm::cross(a, b);
    }

    template < class T >
    inline T normalize(const T& a)
    {
        return glm::normalize(a);
    }

    template < class T >
    inline T length(const T& a)
    {
        return glm::length(a);
    }

    template < class T >
    inline T ceil(const T& a)
    {
        return glm::ceil(a);
    }

    template < class T >
    inline T floor(const T& a)
    {
        return glm::floor(a);
    }

    template < class T >
    inline T abs(const T& a)
    {
        return glm::abs(a);
    }

    template < class T >
    inline T clamp(const T& a, const T& min, const T& max)
    {
        return glm::clamp(a, min, max);
    }

    template < class T, class F >
    inline T mix(const T& a, const T& b, const F& f)
    {
        return glm::mix(a, b, f);
    }

    template < class T, class F >
    inline T lerp(const T& a, const T& b, const F& f)
    {
        return glm::mix(a, b, f);
    }

    template < class T >
    inline T traspose(const T& a)
    {
        return glm::transpose(a);
    }

    template < class T >
    inline T inverse(const T& a)
    {
        return glm::inverse(a);
    }

    template < class T >
    inline T conjugate(const T& a)
    {
        return glm::conjugate(a);
    }

    template < class T >
    inline T diagonal3x3(const T& a)
    {
        return glm::diagonal3x3(a);
    }

    template < class T >
    inline T diagonal4x4(const T& a)
    {
        return glm::diagonal4x4(a);
    }

    template < class T >
    inline T radians(const T& a)
    {
        return glm::radians(a);
    }

    template < class T >
    inline T degrees(const T& a)
    {
        return glm::degrees(a);
    }

    template < class T >
    inline auto row(const T& a, size_t i) -> decltype(glm::row(a, i))
    {
        return glm::row(a, i);
    }

    template < class T >
    inline auto column(const T& a, size_t i) -> decltype(glm::column(a, i))
    {
        return glm::column(a, i);
    }

    template < class T >
    inline auto value_ptr(const T& q_m_v) -> decltype(glm::value_ptr(q_m_v))
    {
        return glm::value_ptr(q_m_v);
    }

    template < class T >
    inline auto quat_cast(const T& q_m) -> decltype(glm::quat_cast(q_m))
    {
        return glm::quat_cast(q_m);
    }

    template < class T >
    inline auto mat3_cast(const T& q_v) -> decltype(glm::mat3_cast(q_v))
    {
        return glm::mat3_cast(q_v);
    }

    template < class T >
    inline auto mat4_cast(const T& q_v) -> decltype(glm::mat4_cast(q_v))
    {
        return glm::mat4_cast(q_v);
    }

    template < class T >
    inline auto to_quat(const T& q_m) -> decltype(glm::toQuat(q_m))
    {
        return glm::toQuat(q_m);
    }

    template < class T >
    inline auto to_mat3(const T& q_v) -> decltype(glm::toMat3(q_v))
    {
        return glm::toMat3(q_v);
    }

    template < class T >
    inline auto to_mat4(const T& q_v) -> decltype(glm::toMat4(q_v))
    {
        return glm::toMat4(q_v);
    }

    template < class T >
    inline auto linear_to_string(const T& q_v) -> decltype(glm::to_string(q_v))
    {
        return glm::to_string(q_v);
    }
}