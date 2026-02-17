
set(SOURCES
    JSONSerializer.cpp
)

set(LIBS)

set(INCLUDES
    ${INCLUDE_DIR}
)

add_library(serialization ${SOURCES})
target_link_libraries(serialization ${LIBS})
target_include_directories(serialization PUBLIC ${INCLUDES})
target_compile_options(serialization PRIVATE ${CSICS_COMPILE_FLAGS})
target_link_options(serialization PRIVATE ${CSICS_LINKER_FLAGS})

