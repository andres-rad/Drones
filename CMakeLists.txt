project(src)

set(SOURCE_FILES
        tipos.h
        tipos.cpp
        campo.h
        campo.cpp
        drone.h
        drone.cpp
        sistema.h
        sistema.cpp
        aux.h
        aux.cpp)

add_library(src ${SOURCE_FILES})