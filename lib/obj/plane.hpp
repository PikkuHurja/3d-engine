#pragma once


#include "gl_mesh_interleaved.hpp"
#include <cassert>
#include <cstddef>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_uint2.hpp>
#include <glm/ext/vector_uint3.hpp>
#include <memory>
#include <sstream>
#include <stdexcept>


struct plane_t : gl_mesh_interleaved_t<3, true, true, true, true, true, true>{
    using mesh_t = gl_mesh_interleaved_t;

    struct verticie{
        glm::vec3 position;
        glm::vec2 uv;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec3 bitangent;
    };

    
    void create(glm::vec3 position, glm::vec2 size, glm::uvec2 vertex_count){ //verticies, per axis
        std::unique_ptr<verticie[]> verticies   = std::make_unique<verticie[]>(vertex_count.x*vertex_count.y);
        size_t indecie_count                    = (vertex_count.x-1)*(vertex_count.y-1)*6;
        std::unique_ptr<uint[]>     indecies    = std::make_unique<uint[]>     (indecie_count);

        for(uint y = 0; y < vertex_count.y; y++){
            for(uint x = 0; x < vertex_count.x; x++){
                auto& e = verticies[x+y*vertex_count.x];
                e.uv        = glm::vec2(x,y)/glm::vec2{vertex_count};
                e.position  =   position + glm::vec3{size * e.uv, 0};
                e.normal    =   glm::vec3{0.0f, 1.0f, 0.0f};    // pointing up
                e.tangent   =   glm::vec3{1.0f, 0.0f, 0.0f};    // along X
                e.bitangent =   glm::vec3{0.0f, 0.0f, 1.0f};    // along Z
            }
        }
                //too lazy to calculate ts
        size_t indecie_index = 0;

        for(uint y = 0; y < vertex_count.y-1; y++){
            for(uint x = 0; x < vertex_count.x-1; x++){
                unsigned int topLeft = y * vertex_count.x + x;
                unsigned int topRight = topLeft + 1;
                unsigned int bottomLeft = (y + 1) * vertex_count.x + x;
                unsigned int bottomRight = bottomLeft + 1;

                indecies[indecie_index++] = topLeft;
                indecies[indecie_index++] = bottomLeft;
                indecies[indecie_index++] = topRight;

                indecies[indecie_index++] = topRight;
                indecies[indecie_index++] = bottomLeft;
                indecies[indecie_index++] = bottomRight;
            }
        }
        
        if(indecie_index != indecie_count)
            throw std::runtime_error((std::ostringstream() << "mismatch indecie_index and indecie_count: " << indecie_index << " != " << indecie_count << '\n').str());

        mesh_t::create(vertex_count.x*vertex_count.y, indecie_count, verticies.get(), indecies.get());
    }


};
