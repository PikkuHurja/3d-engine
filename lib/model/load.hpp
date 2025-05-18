#pragma once

#include "gl/buffer_enums.hpp"
#include "gl/shader_spec.hpp"
#include "gl_mesh_interleaved.hpp"
#include <cassert>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace model {


    
    inline void load(const char* path, std::vector<rt_gl_mesh_interleaved_t>& rt_meshes){
        Assimp::Importer importer{};
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            throw std::runtime_error((std::ostringstream() << "Invalid scene: " << importer.GetErrorString() << "\n").str());

        for(size_t i = 0; i < scene->mNumMeshes; i++){
            auto& gl_mesh = rt_meshes.emplace_back();
            auto& ai_mesh = scene->mMeshes[i];
            assert(ai_mesh);

            gl_mesh.set_vertex_size(3);
            if(ai_mesh->HasPositions()) gl_mesh.enable_attribute(gl::shader_spec::aVertex);
            if(ai_mesh->HasTextureCoords(0)) gl_mesh.enable_attribute(gl::shader_spec::aUV);
            if(ai_mesh->HasNormals()) gl_mesh.enable_attribute(gl::shader_spec::aNormal);
            if(ai_mesh->HasTangentsAndBitangents()) gl_mesh.enable_attribute(gl::shader_spec::aTangent, gl::shader_spec::aBitangent);
            auto vert_count = ai_mesh->mNumVertices;


            std::vector<unsigned int> indecies;
            unsigned int indecie_count = 0;
            const unsigned int* indecie_data = nullptr;


            if(ai_mesh->mNumFaces == 1){
                const aiFace& face = ai_mesh->mFaces[0];
                indecie_count = face.mNumIndices;
                indecie_data = face.mIndices;
            }else if(ai_mesh->mNumFaces > 1){
                for (unsigned int i = 0; i < ai_mesh->mNumFaces; ++i) {
                    const aiFace& face = ai_mesh->mFaces[i];
                    for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                        indecies.push_back(face.mIndices[j]);
                    }
                }
                indecie_count = indecies.size();
                indecie_data = indecies.data();
            }

            gl_mesh.create(
                vert_count,
                ai_mesh->mVertices,
                ai_mesh->mTextureCoords[0],
                ai_mesh->mNormals,
                ai_mesh->mTangents,
                ai_mesh->mBitangents,
                gl::enums::buffer::STATIC_DRAW,
                indecie_count,
                indecie_data
            );

        }
    }




}