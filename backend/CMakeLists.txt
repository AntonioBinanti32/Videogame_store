cmake_minimum_required(VERSION 3.26)
set(CMAKE_TOOLCHAIN_FILE=C:/Users/AntonioBinanti/vcpkg/scripts/buildsystems/vcpkg.cmake)
project(Videogame_store)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)

add_executable(Videogame_store "main.cpp" 
"database/MongoDB.h" "database/MongoDB.cpp"
"handler/Handler.h" "handler/Handler.cpp" 
"exceptions/SignupException.h" "exceptions/LoginException.h" "exceptions/CreateGameException.h" "exceptions/GetGameException.h" "exceptions/UserNotFoundException.h" "exceptions/CreateReservationException.h" "exceptions/GetRecommendationsException.h" "exceptions/SocketException.h" "exceptions/ReviewException.h" "exceptions/ReservationException.h"
"socketTcp/SocketTcp.h" "socketTcp/SocketTcp.cpp" "exceptions/ReviewException.h" "exceptions/ReservationException.h")

find_package(bsoncxx CONFIG REQUIRED)
#se esiste una versione statica della libreria mongocxx (mongo::mongocxx_static) la collega. Altrimenti, collega la versione condivisa (mongo::mongocxx_shared).
target_link_libraries(Videogame_store PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)

find_package(mongocxx CONFIG REQUIRED)
target_link_libraries(Videogame_store PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)
#find_package(mongocxx CONFIG REQUIRED)
#se esiste una versione statica della libreria mongocxx (mongo::mongocxx_static) la collega. Altrimenti, collega la versione condivisa (mongo::mongocxx_shared).
#target_link_libraries(Videogame_store PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>) 

#find_package(bsoncxx CONFIG REQUIRED)
#target_link_libraries(Videogame_store PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)

find_package(unofficial-restbed CONFIG REQUIRED)
target_link_libraries(Videogame_store PRIVATE unofficial::restbed::restbed)

find_package(unofficial-inih CONFIG REQUIRED)
target_link_libraries(Videogame_store PRIVATE unofficial::inih::inireader)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(Videogame_store PRIVATE nlohmann_json::nlohmann_json)

find_package(cpp-jwt CONFIG REQUIRED)
target_link_libraries(Videogame_store PRIVATE cpp-jwt::cpp-jwt)

if(WIN32)
  target_link_libraries(Videogame_store PRIVATE wsock32 ws2_32)
endif()