INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
link_directories(./)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
aux_source_directory(./ DIR_SRCS)
ADD_EXECUTABLE(Scalable_EA ${DIR_SRCS})

target_link_libraries(Scalable_EA mpi EA_CPU DE_CPU CEC2014)

