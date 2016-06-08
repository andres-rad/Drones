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
        auxiliares.h
        auxiliares.cpp)

add_library(src ${SOURCE_FILES})
