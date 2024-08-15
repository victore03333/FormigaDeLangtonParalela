cmake_minimum_required(VERSION 3.28)
project(Trabalho)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML COMPONENTS graphics audio REQUIRED)
find_package(MPI REQUIRED)

add_executable(Trabalho main.cpp
        VLangtonAnt.cpp
        VLangtonAnt.h
        VLangtonParalela.cpp
        VLangtonParalela.h
        MatrizContigua.cpp
        MatrizContigua.h)
target_link_libraries(Trabalho sfml-graphics sfml-audio MPI::MPI_CXX)

# Incluir os diretórios MPI
include_directories(${MPI_INCLUDE_PATH})