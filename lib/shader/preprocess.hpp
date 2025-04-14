#pragma once


#include <bitset>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <istream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace shader {
    namespace _preprocess {
        inline void skip_until_nl(std::istream& is){
            int charachter;

            while((charachter = is.get()) != std::ifstream::traits_type::eof()){
                if(charachter == '\\'){
                    is.ignore(1);
                    continue;
                }
                if(charachter == '\n') 
                    break;
                
            }
        }

        inline void skip_until_bc_end(std::istream& is){
            int charachter;
            bool was_asterix = false;
            while((charachter = is.get()) != std::ifstream::traits_type::eof()){
                if(charachter == '\\'){
                    is.ignore(1);
                    was_asterix = false;
                    continue;
                }
                if (was_asterix && charachter == '/')
                    break;
                if(charachter == '*'){
                    was_asterix = true;
                }
            }
        }

        #define peek_break(var)\
            var = is.peek(); if(var == eof) break;
        #define get_break(var)\
            var = is.get(); if(var == eof) break;


        inline bool read_until_nec(std::istream& is, std::string& to, char c){
            int charachter;
            constexpr int eof = std::ifstream::traits_type::eof();
            do {
                get_break(charachter)
                if(charachter == c)
                    return true;;
                if(charachter == '\\')
                    is.ignore(1);
                else to.push_back(c);

            }while (1);
            return false;
        }
        inline bool read_until_nec(std::istream& is, std::string& to, const std::bitset<256>& set){
            int charachter;
            constexpr int eof = std::ifstream::traits_type::eof();
            do {
                get_break(charachter)
                if(set[charachter])
                    return true;
                if(charachter == '\\')
                    is.ignore(1);
                else to.push_back(charachter);

            }while (1);
            return false;
        }
        inline bool read_until_nec(std::istream& is, std::string& to, const std::unordered_set<char>& set){
            int charachter;
            constexpr int eof = std::ifstream::traits_type::eof();
            do {
                get_break(charachter)
                if(set.contains(charachter))
                    return true;
                if(charachter == '\\')
                    is.ignore(1);
                else to.push_back(charachter);

            }while (1);
            return false;
        }

        
    }

    inline static std::unordered_map<std::string, void(*)(std::istream& is, std::string& to, std::unordered_map<std::string, std::string>& variable_mapping, std::vector<std::filesystem::path>& include_paths)> _M_MacroMap;
    inline void preprocess(const std::filesystem::path& path, std::string& to, std::unordered_map<std::string, std::string>& variable_mapping, std::vector<std::filesystem::path>& include_paths){
        std::ifstream is{path};

        int charachter;
        constexpr int eof = std::ifstream::traits_type::eof();

        do{
            get_break(charachter)

            if(charachter == '\\'){
                to.push_back(charachter);
                get_break(charachter);
                to.push_back(charachter);
                continue;
            }
            /*
            else if(charachter == '/'){
                get_break(charachter)

                if(charachter == '/'){
                    _preprocess::skip_until_nl(is);
                    to.push_back('\n');
                }else if (charachter == '*'){
                    _preprocess::skip_until_bc_end(is);
                }else{
                    to.push_back('/');
                    to.push_back(charachter);
                }

            }
            */
            else if(charachter == '$'){
                peek_break(charachter);
                if(charachter == '{'){
                    // variable
                    is.ignore(1);

                    std::string varname;
                    auto pos = is.tellg();
                    
                    if(!_preprocess::read_until_nec(is, varname, '}'))
                        throw std::runtime_error("Could not find matching '{' (started at [" + std::to_string(pos) + "]) in shader " + path.string());
                    
                    auto it = variable_mapping.find(varname);
                    
                    if(it == variable_mapping.end())
                        throw std::runtime_error("Could not find variable '" + varname +"' in variable_mapping pool for shader " + path.string());
                    
                    to+=it->second;

                }else{
                    std::string varname;
                    is >> varname;

                    auto it = _M_MacroMap.find(varname);
                    if(it == _M_MacroMap.end())
                        throw std::runtime_error("Could not find macro '" + varname +"' in _M_MacroMap pool for shader " + path.string());
                    
                    if(it->second)
                        it->second(is, to, variable_mapping, include_paths);
                }
            }else{
                to.push_back(charachter);
            }

        } while(1);
    }
}
