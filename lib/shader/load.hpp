#pragma once


#include "gl/program.hpp"
#include "gl/shader.hpp"
#include "gl/shader_enums.hpp"
#include "preprocess.hpp"
#include <filesystem>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>
namespace shader {

    inline static std::vector<std::filesystem::path> _DefaultIncludePaths{
        "./ass/shaders"
    };


    enum _ShaderExtensionType{
        VERTEX,
        FRAGMENT,
        COMPUTE,
        GEOMETRY,
        TESSELATION_CONTROL,
        TESSELATION_EVAL,
        COUNT,
    };
    inline static gl::enums::shader::type ext_type_to_shader_type[COUNT]{
        gl::enums::shader::VERTEX,
        gl::enums::shader::FRAGMENT,
        gl::enums::shader::COMPUTE,
        gl::enums::shader::GEOMETRY,
        gl::enums::shader::TESSELATION_CONTROL,
        gl::enums::shader::TESSELATION_EVAL,
    };
    inline static std::unordered_set<std::string> _ShaderExtensions[COUNT]{

        std::unordered_set<std::string>{
            ".vertex",
            ".vert",
            ".vrt",
            ".vtx",
        },
        std::unordered_set<std::string>{
            ".fragment",
            ".frag",
            ".frg",
            ".fgm",
        },
        std::unordered_set<std::string>{
            ".compute",
            ".comp",
            ".cmp",
        },
        std::unordered_set<std::string>{
            ".geometry",
            ".geom",
            ".geo",
            ".gmt",
        },
        std::unordered_set<std::string>{
            ".tesselation_control",
            ".tessc",
            ".tssc",
            ".tsc",
        },
        std::unordered_set<std::string>{
            ".tesselation_eval",
            ".tessev",
            ".tsse",
            ".tse",
        },
    };

    

    inline gl::shader load_shader(const std::filesystem::path& path, std::unordered_map<std::string, std::string>& variable_mapping, std::vector<std::filesystem::path> &include_paths){
        if(!path.has_extension()) return {nullptr};
        std::string ext = path.extension();
        _ShaderExtensionType ext_type = COUNT;
        for(size_t i = 0; i < COUNT; i++){
            if(_ShaderExtensions[i].contains(ext)){
                ext_type = static_cast<_ShaderExtensionType>(i);
                break;
            }
        }
        if(ext_type == COUNT){
            std::cerr << "Could not find the type for shader extension '" << path.extension() << "'\n";
            return {};
        };
        std::cout << "Loading shader " << path << " of type: " << ext_type << '\n';

        std::string output;
        preprocess(path, output, variable_mapping, include_paths);
        std::cout << "Source:\n\e[1m" << output << "\e[22m\n";
        
        return gl::shader{
            ext_type_to_shader_type[ext_type],
            output
        };
    }


    inline gl::program load_compute(const std::filesystem::path& path, std::unordered_map<std::string, std::string>& variable_mapping, std::vector<std::filesystem::path> &include_paths);

        //non recursive, because.... yeah loop di loop
    inline gl::program load_directory(const std::filesystem::path& path, std::unordered_map<std::string, std::string>& variable_mapping, std::vector<std::filesystem::path>& include_paths){
        std::cout << "load_directory\n";

        if(!std::filesystem::is_directory(path)) //only compute shaders may be alone
            return load_compute(path, variable_mapping, include_paths);

        std::vector<gl::shader> shaders;
        for(auto it = std::filesystem::directory_iterator{path}; it != std::filesystem::directory_iterator{}; it++){
            const auto& ent = *it;
            if(!ent.is_regular_file() && !ent.is_symlink()) continue;
            shaders.emplace_back(load_shader(ent, variable_mapping, include_paths));
        }
        gl::program prog;
        for(auto& e : shaders){
            prog.attach(e);
        }
        auto error = prog.link();
        if(error.empty())
            return prog;
        throw std::runtime_error(error);
    }


    inline gl::program load_compute(const std::filesystem::path& path, std::unordered_map<std::string, std::string>& variable_mapping, std::vector<std::filesystem::path> &include_paths){
        std::cout << "load_compute\n";

        if(std::filesystem::is_directory(path))
            load_directory(path, variable_mapping,include_paths);

        return gl::program{load_shader(path, variable_mapping, include_paths)};
    }

    inline gl::program load(const std::filesystem::path& path, std::unordered_map<std::string, std::string>& variable_mapping, std::vector<std::filesystem::path>& include_paths ){
        std::cout << "load\n";
        if(std::filesystem::is_directory(path)) //only compute shaders may be alone
            return load_directory(path, variable_mapping, include_paths);
        return load_compute(path, variable_mapping, include_paths);
    }
}