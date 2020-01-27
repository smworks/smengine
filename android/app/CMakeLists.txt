cmake_minimum_required(VERSION 3.4.1)

file(GLOB luafiles "../../dependencies/includes/lua/*.c")
file(GLOB libpngfiles "../../dependencies/includes/libpng/*.c" "../../dependencies/includes/zlib/*.c")
file(GLOB files
    "../../main_android.cpp"
    "../../ghost/*.cpp"
    "../../ghost/Resources/*.cpp"
    "../../ghost/Network/*.cpp"
    "../../ghost/Extensions/*.cpp"
    "../../ghost/Multiplatform/*.cpp"
    "../../ghost/Multiplatform/Android/*.cpp"
    "../../dependencies/includes/sqlite/sqlite3.c"
    "../../dependencies/includes/fmod/android/*.hpp"
    "../../dependencies/includes/fmod/android/*.h"
    "../../dependencies/includes/freetype/*.c"
    "../../dependencies/includes/bullet/BulletDynamics/Vehicle/*.cpp"
    "../../dependencies/includes/bullet/BulletDynamics/ConstraintSolver/*.cpp"
    "../../dependencies/includes/bullet/BulletDynamics/Dynamics/*.cpp"
    "../../dependencies/includes/bullet/BulletCollision/CollisionDispatch/*.cpp"
    "../../dependencies/includes/bullet/BulletCollision/CollisionShapes/*.cpp"
    "../../dependencies/includes/bullet/BulletCollision/NarrowPhaseCollision/*.cpp"
    "../../dependencies/includes/bullet/BulletCollision/Gimpact/*.cpp"
    "../../dependencies/includes/bullet/BulletCollision/BroadphaseCollision/*.cpp"
    "../../dependencies/includes/bullet/BulletSoftBody/*.cpp"
    "../../dependencies/includes/bullet/LinearMath/*.cpp"
)

#add_library(fmod SHARED IMPORTED)
#set_target_properties(fmod PROPERTIES IMPORTED_LOCATION
#    ${CMAKE_SOURCE_DIR}/../../dependencies/libs/android/${ANDROID_ABI}/libfmodex.so)

add_library(lua SHARED ${luafiles})
add_library(libpng SHARED ${libpngfiles})
add_library(smengine SHARED ${files})

find_library(log-lib log)

#target_include_directories(
#    smengine PRIVATE ${CMAKE_SOURCE_DIR}/../../dependencies/includes/fmod/android)

target_link_libraries(lua)
target_link_libraries(libpng)
target_link_libraries(smengine libpng lua ${log-lib} EGL GLESv2)