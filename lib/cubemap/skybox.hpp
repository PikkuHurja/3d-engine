#pragma once


#include "cubemap/cube-map.hpp"
#include "gl/draw_enums.hpp"
#include "gl/program.hpp"
#include "gl_mesh.hpp"
#include "shader/load.hpp"
#include <cstddef>
#include <filesystem>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
struct skybox_t : cube_map{
    /*
        we need to make le box
    */
    inline static constexpr glm::vec3 skybox_vertices[] = {
        // positions          
        {-1.0f,  1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f},

        {-1.0f, -1.0f,  1.0f},
        {-1.0f, -1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f, -1.0f},
        {-1.0f,  1.0f,  1.0f},
        {-1.0f, -1.0f,  1.0f},

        {1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f,  1.0f},
        {1.0f,  1.0f,  1.0f},
        {1.0f,  1.0f,  1.0f},
        {1.0f,  1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},

        {-1.0f, -1.0f,  1.0f},
        {-1.0f,  1.0f,  1.0f},
        {1.0f,  1.0f,  1.0f},
        {1.0f,  1.0f,  1.0f},
        {1.0f, -1.0f,  1.0f},
        {-1.0f, -1.0f,  1.0f},

        {-1.0f,  1.0f, -1.0f},
        {1.0f,  1.0f, -1.0f},
        {1.0f,  1.0f,  1.0f},
        {1.0f,  1.0f,  1.0f},
        {-1.0f,  1.0f,  1.0f},
        {-1.0f,  1.0f, -1.0f},

        {-1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f,  1.0f},
        {1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f,  1.0f},
        {1.0f, -1.0f,  1.0f},
    };
    inline static constexpr uint skybox_vertex_count = sizeof(skybox_vertices)/sizeof(glm::vec3);
    
    inline static gl_mesh<mesh_property::HAS_VERTICIES> unit_cube{};
    
    inline static gl::program skybox_program{nullptr};
    inline static std::filesystem::path skybox_program_path = "ass/shaders/skybox";
    inline static const char* _N_UnTranslatedCameraMatrix = "projection_view";
    inline static const char* _N_Albedo = "albedo";
    inline static const char* _N_CubemapTexture = "skybox_cubemap";
    inline static int _L_UnTranslatedCameraMatrix = -1;
    inline static int _L_Albedo = -1;
    inline static int _L_CubemapTexture = -1;

    inline static void refresh_shader(){
        skybox_program = shader::load(skybox_program_path);
        _L_UnTranslatedCameraMatrix = skybox_program.location(_N_UnTranslatedCameraMatrix);
        _L_Albedo = skybox_program.location(_N_Albedo);
        _L_CubemapTexture = skybox_program.location(_N_CubemapTexture);
    }
    inline static void create_cube(){
        unit_cube.create(skybox_vertex_count, nullptr, skybox_vertices);
    }
    inline static void draw_cube(){
        unit_cube.draw(gl::enums::TRIANGLES, 0, skybox_vertex_count);
    }

    inline void draw(const glm::mat4& non_translated_camera_matrix, const glm::vec4& albedo = glm::vec4(1)){
        skybox_program.use();
        skybox_program.set(_L_UnTranslatedCameraMatrix, non_translated_camera_matrix);
        skybox_program.set(_L_Albedo, albedo);
        
        texture().bind(0);
        skybox_program.set(_L_CubemapTexture, 0);
        draw_cube();
    }
    
    skybox_t(std::nullptr_t = nullptr):cube_map(nullptr){}

};