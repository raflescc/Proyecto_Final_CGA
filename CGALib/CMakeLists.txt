add_definitions(-DBUILDING_DLL) # Define a macro indicating DLL import

file(GLOB CGALib_SRC
     "./include/Headers/*.h"
     "./src/*.cpp"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
     include_directories(
          CGALib 
          "./include/" 
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
     )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
     include_directories(
          CGALib 
          "./include/" 
          "/usr/include/freetype2" 
     )
endif()
add_library(CGALib SHARED ${CGALib_SRC})
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
     set_target_properties(CGALib
          PROPERTIES
          RUNTIME_OUTPUT_DIRECTORY "../../bin"
     )
     target_link_libraries(CGALib opengl32.lib libglfw3dll.dll.a libglew32.dll.a libassimp.dll.a libfreetype.dll.a)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
     target_link_libraries(CGALib glut glfw GLEW GLU GL freeimage assimp freetype)
endif()