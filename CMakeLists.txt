cmake_minimum_required(VERSION 3.12)

project(pokerengine VERSION 1.9.1)

include(cmake/Packages.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(LIBRARY_NAME pokerengine_core)

set(POKERENGINE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/pokerengine)
set(POKERENGINE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

include_directories(${PROJECT_NAME} ${POKERENGINE_INCLUDE_DIR})

set(SOURCE_FILES ${POKERENGINE_SOURCE_DIR}/pokerengine.cpp)
set(HEADER_FILES
        ${POKERENGINE_INCLUDE_DIR}/bits.hpp
        ${POKERENGINE_INCLUDE_DIR}/card/card.hpp
        ${POKERENGINE_INCLUDE_DIR}/card/cards.hpp
        ${POKERENGINE_INCLUDE_DIR}/card/hand.hpp
        ${POKERENGINE_INCLUDE_DIR}/constants.hpp
        ${POKERENGINE_INCLUDE_DIR}/engine/action.hpp
        ${POKERENGINE_INCLUDE_DIR}/engine/engine.hpp
        ${POKERENGINE_INCLUDE_DIR}/engine/player.hpp
        ${POKERENGINE_INCLUDE_DIR}/engine/pot.hpp
        ${POKERENGINE_INCLUDE_DIR}/engine/round.hpp
        ${POKERENGINE_INCLUDE_DIR}/enums.hpp
        ${POKERENGINE_INCLUDE_DIR}/evaluator/evaluation.hpp
        ${POKERENGINE_INCLUDE_DIR}/evaluator/evaluation_result.hpp
        ${POKERENGINE_INCLUDE_DIR}/evaluator/lookup_tables.hpp
        ${POKERENGINE_INCLUDE_DIR}/evaluator/result.hpp
        ${POKERENGINE_INCLUDE_DIR}/pokerengine.hpp
        ${POKERENGINE_INCLUDE_DIR}/python/pycard.hpp
        ${POKERENGINE_INCLUDE_DIR}/python/pyconstants.hpp
        ${POKERENGINE_INCLUDE_DIR}/python/pyengine.hpp
        ${POKERENGINE_INCLUDE_DIR}/python/pyenums.hpp
        ${POKERENGINE_INCLUDE_DIR}/python/pyevaluation.hpp
        ${POKERENGINE_INCLUDE_DIR}/python/pyexceptions.hpp
        ${POKERENGINE_INCLUDE_DIR}/python/pypokerengine.hpp
        ${POKERENGINE_INCLUDE_DIR}/python/python.hpp
        ${POKERENGINE_INCLUDE_DIR}/string.hpp
        ${POKERENGINE_INCLUDE_DIR}/types.hpp
        ${POKERENGINE_INCLUDE_DIR}/vector.hpp)

find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

python_add_library(${LIBRARY_NAME} MODULE ${SOURCE_FILES} ${HEADER_FILES} WITH_SOABI)
target_link_libraries(${LIBRARY_NAME} PRIVATE pybind11::headers magic_enum::magic_enum)
target_compile_definitions(${LIBRARY_NAME} PRIVATE VERSION_INFO=${PROJECT_VERSION})

install(TARGETS ${LIBRARY_NAME} DESTINATION pokerengine)
