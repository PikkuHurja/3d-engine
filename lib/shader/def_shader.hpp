#pragma once
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/facilities/is_empty_variadic.hpp>
#include <boost/preprocessor/tuple/size.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/comparison/equal.hpp>

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/comma_if.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <glm/ext/vector_uint3.hpp>
#include <sys/types.h>
#include <filesystem>
#include <gl/shader_enums.hpp>
#include <gl/program_enums.hpp>
#include <gl/program.hpp>
#include <fstream>
#include <appstate.hpp>



#define PRG_PROGRAM_POINTER      _S_Program
#define PRG_WORKGROUPS           _N_WorkGroups
#define PRG_NAME(property_name)      _N_ ## property_name
#define PRG_LOCATION(property_name)  _L_ ## property_name
#define PRG_SET_FN(property_name)    set_ ## property_name
#define PRG_SH_PATH(index)  _P_Path_ ## index
#define PRG_SH_TYPE(index)  _T_Path_ ## index
#define PRG_TYPE(program_name) program_name ## _t



    //////////////////////////// WORKGROUP ////////////////////////////
#define IS_EMPTY_TUPLE_IMPL(...) BOOST_PP_EQUAL(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 0)
#define IS_EMPTY_TUPLE(tuple) IS_EMPTY_TUPLE_IMPL tuple

#define PRG_INIT_WORKGROUP_BRK(x,y,z)          {x,y,z}

#define PRG_INIT_WORKGROUP_0()          (0,0,0)
#define PRG_INIT_WORKGROUP_1(x)         (x,1,1)
#define PRG_INIT_WORKGROUP_2(x, y)      (x,y,1)
#define PRG_INIT_WORKGROUP_3(x, y, z)   (x,y,z)


#define PRG_INIT_DISPATCH(size)  BOOST_PP_CAT(PRG_INIT_WORKGROUP_, size)
#define PRG_INIT_WORKGROUP(tuple)\
    BOOST_PP_EXPAND(PRG_INIT_WORKGROUP_BRK BOOST_PP_IF(IS_EMPTY_TUPLE(tuple), PRG_INIT_WORKGROUP_0(), PRG_INIT_DISPATCH(BOOST_PP_EXPAND(BOOST_PP_TUPLE_SIZE(tuple))) tuple))


    //////////////////////////// PROPERTY ////////////////////////////

        ////////////// AS ARGUMENT //////////////
#define PRG_DEFINE_PROPERTY_AS_ARGUMENT(property_name, propert_type)\
    const propert_type& property_name
#define PRG_DEFINE_PROPERTY_AS_ARGUMENT_EXPAND(r, data, index, elem) BOOST_PP_COMMA_IF(index) PRG_DEFINE_PROPERTY_AS_ARGUMENT elem
#define PRG_DEFINE_PROPERTIES_AS_ARGUMENTS(properties)\
    BOOST_PP_SEQ_FOR_EACH_I(PRG_DEFINE_PROPERTY_AS_ARGUMENT_EXPAND, _, properties)



        ////////////// DEFINE //////////////
#define PRG_DEFINE_PROPERTY(property_name, propert_type)\
    inline static const char * PRG_NAME(property_name) = #property_name;\
    inline static uint PRG_LOCATION(property_name) = -1;
#define PRG_DEFINE_PROPERTY_EXPAND(r, data, elem) PRG_DEFINE_PROPERTY elem
#define PRG_DEFINE_PROPERTIES(properties) \
    BOOST_PP_SEQ_FOR_EACH(PRG_DEFINE_PROPERTY_EXPAND, _, properties)

        ////////////// DEFINE FUNCTIONS //////////////
#define PRG_DEFINE_PROPERTY_SET_FUNCTION(property_name, property_type)\
    inline static void PRG_SET_FN(property_name) (const property_type& v){PRG_PROGRAM_POINTER->set(PRG_LOCATION(property_name), v);}
#define PRG_DEFINE_PROPERTY_SET_FUNCTION_EXPAND(r, data, elem) PRG_DEFINE_PROPERTY_SET_FUNCTION elem
#define PRG_DEFINE_PROPERTY_SET_FUNCTIONS(properties)\
    BOOST_PP_SEQ_FOR_EACH(PRG_DEFINE_PROPERTY_SET_FUNCTION_EXPAND, _, properties)

        ////////////// SET ALL //////////////
#define PRG_SET_PROPERTY(property_name, property_type) PRG_SET_FN(property_name) (property_name);
#define PRG_SET_PROPERTY_EXPAND(r, data, elem) PRG_SET_PROPERTY elem
#define PRG_SET_PROPERTIES(properties)\
    BOOST_PP_SEQ_FOR_EACH(PRG_SET_PROPERTY_EXPAND, _, properties)

        ////////////// INITIALIZE //////////////
#define PRG_INITIALIZE_PROPERTY(property_name, propert_type)\
    PRG_LOCATION(property_name) = _S_Program->location(_N_ ## property_name);
#define PRG_INITIALIZE_PROPERTY_EXPAND(r, data, elem) PRG_INITIALIZE_PROPERTY elem
#define PRG_INITIALIZE_PROPERTIES(properties)\
    BOOST_PP_SEQ_FOR_EACH(PRG_INITIALIZE_PROPERTY_EXPAND, _, properties)


    //////////////////////////// PATHS ////////////////////////////

        ////////////// DEFINE //////////////
#define PRG_DEFINE_A_PATH(r, data, index, elem)\
    inline static std::filesystem::path PRG_SH_PATH(index) = BOOST_PP_TUPLE_ELEM(2, 0, elem); \
    inline static gl::enums::shader::type PRG_SH_TYPE(index) = BOOST_PP_TUPLE_ELEM(2, 1, elem);
#define PRG_DEFINE_PATHS(shader_path_tuple)\
    BOOST_PP_SEQ_FOR_EACH_I(PRG_DEFINE_A_PATH, _, shader_path_tuple)

        ////////////// CREATE SHADER //////////////
#define PRG_MK_SH(index)\
    gl::shader{PRG_SH_TYPE(index), (std::ostringstream() << std::ifstream(PRG_SH_PATH(index)).rdbuf()).str()},
#define PRG_MK_SH_EXPAND(r, data, index, elem)\
    PRG_MK_SH(index)

        ////////////// CREATE PROGRAM //////////////
#define PRG_INIT_PRG(shader_path_tuple)\
    BOOST_PP_SEQ_FOR_EACH_I(PRG_MK_SH_EXPAND, _, shader_path_tuple)



    //////////////////////////// CLASS ////////////////////////////

#define PRG_DEFINE(program_name, work_groups, shaders, properties, refresh_code)\
    struct PRG_TYPE(program_name) {\
        inline static std::shared_ptr<gl::program>  PRG_PROGRAM_POINTER;\
        inline static glm::uvec3                    PRG_WORKGROUPS PRG_INIT_WORKGROUP(work_groups);\
        PRG_DEFINE_PATHS(shaders)           \
        PRG_DEFINE_PROPERTIES(properties)   \
        PRG_DEFINE_PROPERTY_SET_FUNCTIONS(properties) \
        inline static void refresh_shader(){appstate_t::gl_t::after_gl([](){\
           PRG_PROGRAM_POINTER .reset(new gl::program{PRG_INIT_PRG(shaders)});\
            PRG_INITIALIZE_PROPERTIES(properties)\
           refresh_code;\
        });}\
        inline static void use(){PRG_PROGRAM_POINTER->use();}\
        inline static void use(PRG_DEFINE_PROPERTIES_AS_ARGUMENTS(properties)){\
            use();\
            PRG_SET_PROPERTIES(properties)\
        }\
        inline static void dispatch(glm::uvec3 n){PRG_PROGRAM_POINTER->dispatch(n, PRG_WORKGROUPS);}\
        inline static void dispatch(glm::uvec2 n){PRG_PROGRAM_POINTER->dispatch(n, PRG_WORKGROUPS);}\
        inline static void dispatch(uint n)      {PRG_PROGRAM_POINTER->dispatch(n, PRG_WORKGROUPS);}\
        PRG_TYPE(program_name)() = delete;                                    \
        ~PRG_TYPE(program_name)() = delete;                                    \
    };

PRG_DEFINE(test_program, (1u, 1, 1), (("path", gl::enums::shader::FRAGMENT)), ((a, uint))((b, float)), {});
// Expands to
