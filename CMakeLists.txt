cmake_minimum_required(VERSION 3.16)
project(Breakout C)

set(CMAKE_C_STANDARD 11)

# Ruta a la carpeta donde descomprimiiste SDL3
set(SDL3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL3")

# Agregar tu ejecutable
add_executable(Breakout 
    src/main.c
    src/graphics.c
    src/breakout.c
)

target_include_directories(Breakout PRIVATE ${SDL3_DIR}/include)

# Vincular la librería de SDL3 según el sistema operativo / compilador
if(MSVC)
    target_link_libraries(Breakout PRIVATE ${SDL3_DIR}/lib/x64/SDL3.lib)
elseif(MINGW)
    target_link_libraries(Breakout PRIVATE ${SDL3_DIR}/lib/x64/libSDL3.dll.a)
endif()

# Copiar la DLL al directorio de salida para que pueda ejecutarse
add_custom_command(TARGET Breakout POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<IF:$<BOOL:${MSVC}>,${SDL3_DIR}/lib/x64/SDL3.dll,${SDL3_DIR}/bin/SDL3.dll>"
    $<TARGET_FILE_DIR:Breakout>
)