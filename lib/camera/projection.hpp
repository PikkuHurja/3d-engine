#pragma once
#include "orthographic.hpp"
#include "perspective.hpp"
#include <variant>

struct projection_t : std::variant<std::nullptr_t, orthographic, perspective_t>{
    enum projection_type{
        NONE,
        ORTHOGRAPHIC,
        PERSPECTIVE,
    };

    projection_type type()const{
        return static_cast<enum projection_type>(v::index());
    }
    using v = std::variant<std::nullptr_t, orthographic, perspective_t>;
    projection_t():v{perspective_t::make_default()}{}
    using v::variant;

    glm::mat4 projection_matrix()const{
        switch (type()) {
        case ORTHOGRAPHIC: return std::get<orthographic>(*this).matrix();
        case PERSPECTIVE:  return std::get<perspective_t>(*this).matrix();
        default:
        case NONE: 
            return glm::mat4(1.f);
        }
    }
};
