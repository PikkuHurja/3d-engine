#pragma once

#include <glm/ext.hpp>
#include <glm/ext/vector_uint2.hpp>


namespace obj {
    struct terrain_t{
            //assume 1u = 1m
        inline static glm::uvec2 _S_ChunkSize       {64u};
        inline static glm::uvec2 _S_ChunkMeshSize   {64u}; //verticies per chunck

        
    };
}