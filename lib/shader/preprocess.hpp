#pragma once


#include <bitset>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <istream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

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

inline void read_until_nec(std::istream& is, std::string& to, char c){
    int charachter;
    constexpr int eof = std::ifstream::traits_type::eof();
    do {
        get_break(charachter)
        if(charachter == c)
            break;
        if(charachter == '\\')
            is.ignore(1);
        else to.push_back(c);

    }while (1);

}


inline void load(const std::filesystem::path& path, std::string& to, std::unordered_map<std::string, std::string>& variable_mapping){
    std::ifstream is{path};

    int charachter;
    constexpr int eof = std::ifstream::traits_type::eof();

    do{
        get_break(charachter)

        if(charachter == '/'){
            peek_break(charachter)

            if(charachter == '/'){
                is.ignore(1);
                skip_until_nl(is);
            }else if (charachter == '*'){
                is.ignore(1);
                skip_until_bc_end(is);
            }else{
                to.push_back(charachter);
                continue;
            }

        }else if(charachter == '$'){
            peek_break(charachter);
            if(charachter == '{'){
                // variable
                std::string varname;
                is.ignore(1);
                read_until_nec(is, varname, '}');
                auto it = variable_mapping.find(varname);
                if(it == variable_mapping.end())
                    throw std::runtime_error("Could not find variable '" + varname +"' in variable_mapping pool for shader " + path.string());
                to+=it->second;
            }else if (charachter == 'i'){
                

            }
        }

    } while(1);
}
