cmake_minimum_required(VERSION 3.4.1)

file(GLOB luafiles "../../dependencies/includes/lua/*.c")
file(GLOB files
    "../../main_android.cpp"
    "../../ghost/*.cpp"
    "../../ghost/Resources/*.cpp"
    "../../ghost/Network/*.cpp"
    "../../ghost/Multiplatform/*.cpp"
    "../../ghost/Multiplatform/Android/*.cpp"
    "../../dependencies/includes/sqlite/sqlite3.c"
)

#add_library(fmod SHARED IMPORTED)
#set_target_properties(fmod PROPERTIES IMPORTED_LOCATION
#    ${CMAKE_SOURCE_DIR}/../../dependencies/libs/android/${ANDROID_ABI}/libfmodex.so)
add_library(lua SHARED ${luafiles})
add_library(smengine SHARED ${files})

find_library(log-lib log)

#target_include_directories(
#    smengine PRIVATE ${CMAKE_SOURCE_DIR}/../../dependencies/includes/fmod/android)

target_link_libraries(lua)
target_link_libraries(smengine lua ${log-lib} EGL GLESv2)