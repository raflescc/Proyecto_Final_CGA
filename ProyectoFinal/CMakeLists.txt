add_definitions(-DBUILDING_DLL) # Define a macro indicating DLL import


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Set the directory where DLLs are located
    set(CMAKE_DLLTOOLCHAIN_PREFIX "../dlls")
    include_directories(
        ProyectoFinal 
        ${CMAKE_SOURCE_DIR}/CGALib/include
        "/usr/include/freetype2" 
        "${CMAKE_SOURCE_DIR}/external/assimp/include"
        "${CMAKE_SOURCE_DIR}/external/glfw/include"
        "${CMAKE_SOURCE_DIR}/external/glm"
        "${CMAKE_SOURCE_DIR}/external/glew/include"
        "${CMAKE_SOURCE_DIR}/external/FreeImage/include"
        "${CMAKE_SOURCE_DIR}/external/OpenAL/include"
        "${CMAKE_SOURCE_DIR}/external/Freetype/include"
        "${CMAKE_SOURCE_DIR}/external/freealut/include"
    )
    link_directories(
        "${CMAKE_SOURCE_DIR}/external/assimp/lib"
        "${CMAKE_SOURCE_DIR}/external/glfw/lib"
        "${CMAKE_SOURCE_DIR}/external/Freetype/lib"
        "${CMAKE_SOURCE_DIR}/external/FreeImage/lib"
        "${CMAKE_SOURCE_DIR}/external/glew/lib"
        "${CMAKE_SOURCE_DIR}/external/freealut/lib"
        "${CMAKE_SOURCE_DIR}/external/OpenAL/lib/x64"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include_directories(${CMAKE_SOURCE_DIR}/CGALib/include)
endif()

add_executable(ProyectoFinal ./src/main.cpp)

set_target_properties(ProyectoFinal 
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "../../bin"
)


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(ProyectoFinal opengl32.lib libglfw3dll.dll.a libglew32.dll.a libassimp.dll.a libfreetype.dll.a libalut.dll.a OpenAL32.lib CGALib)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(ProyectoFinal glut glfw GLEW GLU GL freeimage assimp freetype alut openal CGALib)
endif()