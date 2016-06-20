cmake_minimum_required(VERSION 3.3)
project(sdf2postgres)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -march=native")

# If find_package does not work, comment it and adapt the commented lines below
find_package(PostgreSQL REQUIRED)
# include_directories(/usr/pgsql-9.5/include)
# target_link_libraries(sdf2postgres /usr/pgsql-9.5/lib/libpq.so)

set(SOURCE_FILES main.cc Database.cc Database.h Csv.cc Csv.h Output.cc Output.h)
add_executable(sdf2postgres ${SOURCE_FILES})

