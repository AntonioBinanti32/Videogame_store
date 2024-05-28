cmake_minimum_required(VERSION 3.10)
project(Videogame_store)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 20)

add_executable(Videogame_store "main.cpp" "database/MongoDB.cpp" "database/MongoDB.cpp" "server/server.h" "server/server.cpp" "handler/handler.h" "handler/handler.cpp" "exceptions/SignupException.h" "exceptions/LoginException.h" "exceptions/CreateGameException.h" "exceptions/GetGameException.h" "exceptions/UserNotFoundException.h" "exceptions/CreateReservationException.h" "exceptions/GetRecommendationsException.h")

find_package(mongocxx CONFIG REQUIRED)
#se esiste una versione statica della libreria mongocxx (mongo::mongocxx_static) la collega. Altrimenti, collega la versione condivisa (mongo::mongocxx_shared).
target_link_libraries(Videogame_store PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>) 

find_package(unofficial-restbed CONFIG REQUIRED)
target_link_libraries(Videogame_store PRIVATE unofficial::restbed::restbed)

find_package(unofficial-inih CONFIG REQUIRED)
target_link_libraries(Videogame_store PRIVATE unofficial::inih::inireader)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(Videogame_store PRIVATE nlohmann_json::nlohmann_json)

find_package(cpp-jwt CONFIG REQUIRED)
target_link_libraries(Videogame_store PRIVATE cpp-jwt::cpp-jwt)