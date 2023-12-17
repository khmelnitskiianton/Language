include_directories(./hdr) 

set(PREF_SRC "src")
set(PREF_HDR "hdr")

FILE(GLOB SRC ${PREF_SRC}/*.cpp)
FILE(GLOB HDR ${PREF_HDR}/*.h)

set(NAME_EXE "frontend.exe")
add_executable(${NAME_EXE} ${SRC} ${HDR})