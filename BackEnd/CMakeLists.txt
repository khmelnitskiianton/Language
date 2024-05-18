include_directories(./include) 

set(PREF_SRC "src")
set(PREF_HDR "include")

FILE(GLOB SRC ${PREF_SRC}/*.cpp)
FILE(GLOB HDR ${PREF_HDR}/*.h)

set(NAME_EXE "backend.elf")
add_executable(${NAME_EXE} ${SRC} ${SRC_LIB} ${HDR} ${HDR_LIB})