#pragma once

#include <glm/ext.hpp>
#include <glm/ext/vector_uint2.hpp>

#include "gl/draw_enums.hpp"
#include "gl_mesh.hpp"

namespace obj {

    struct terrain_t{
            //assume 1u = 1m
        inline static constexpr glm::uvec2 _S_ChunkSize       {64u};    //units per chunc
        inline static constexpr glm::uvec2 _S_ChunkMeshSize   {64u};    //verticies per chunck
        inline static constexpr uint       _S_VertexCount     {_S_ChunkMeshSize.x*_S_ChunkMeshSize.y};
        inline static constexpr gl::enums::drawmode _S_DrawMode = gl::enums::LINE_STRIP;
        gl_mesh<HAS_VERTICIES, HAS_TEXTUREMAP>   _M_Mesh;
        glm::uvec2              _M_Position;
        

        void draw(){
            _M_Mesh.draw(_S_DrawMode, 0, _S_VertexCount);
        }
        
            //make an instancer
        //void draw_all(){
        //    _M_Mesh.draw(_S_DrawMode, 0, _S_VertexCount);
        //}

        void generate(glm::uvec2 seed, glm::uvec2 position){
            glm::vec2 verticies[_S_VertexCount]={{0,0}};
            glm::vec2 uv[_S_VertexCount]={{0,0}};
            _M_Mesh.create(_S_VertexCount, nullptr, verticies);

        
        }

        
    };
}