#pragma once

#include <glm/ext.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_uint2.hpp>

#include "gl/draw_enums.hpp"
#include "gl/shader_enums.hpp"
#include "gl_mesh.hpp"
#include "shader/def_shader.hpp"

namespace obj {

    struct terrain_t{



            //assume 1u = 1m
        inline static constexpr glm::uvec2 _S_ChunkSize       {64u};    //units per chunc
        inline static constexpr glm::uvec2 _S_ChunkMeshSize   {64u};    //verticies per chunck
        inline static constexpr uint       _S_VertexCount     {_S_ChunkMeshSize.x*_S_ChunkMeshSize.y};
        inline static constexpr gl::enums::drawmode _S_DrawMode = gl::enums::TRIANGLES;
        gl_mesh<HAS_VERTICIES, HAS_TEXTUREMAP>   _M_Mesh    ;
        glm::uvec2                               _M_Position;
        
        PRG_DEFINE(generate_verticies, (_S_ChunkMeshSize.x, _S_ChunkMeshSize.y), (("path", gl::enums::shader::COMPUTE)), ((chunk_size, glm::uvec2)), {})

        void draw(){
            _M_Mesh.draw(_S_DrawMode, 0, _S_VertexCount);
        }
            //make an instancer
        //void draw_all(){
        //    _M_Mesh.draw(_S_DrawMode, 0, _S_VertexCount);
        //}

        void generate(uint seed, glm::uvec2 position){
            //glm::vec3 verticies[_S_VertexCount]={{0,0,0}};
            //glm::vec2 uv[_S_VertexCount]={{0,0}};
            _M_Mesh.create<glm::vec3>(_S_VertexCount);

        }

        
    };
}