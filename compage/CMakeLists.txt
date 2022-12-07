# Aggregate source files
AUX_SOURCE_DIRECTORY(src SOURCE_FILES)
AUX_SOURCE_DIRECTORY(lib/ini SOURCE_FILES)

# Compile static/shared libraries using given sources
add_library(compage OBJECT ${SOURCE_FILES})
add_library(compage_static STATIC $<TARGET_OBJECTS:compage>)
add_library(compage_shared SHARED $<TARGET_OBJECTS:compage>)

# Properties
set_property(TARGET compage PROPERTY POSITION_INDEPENDENT_CODE 1)
set_property(TARGET compage_static PROPERTY OUTPUT_NAME compage)
set_property(TARGET compage_shared PROPERTY OUTPUT_NAME compage)

# Installation rules
install(TARGETS compage_static DESTINATION lib/edi/)
install(TARGETS compage_shared DESTINATION lib/edi/)
install(
  FILES 
    inc/compage.h
    inc/compage_register.h
    inc/compage_hash.h
    inc/compage_types.h
    inc/compage_macro.h
  DESTINATION
    include/edi)


# Add library
target_link_libraries(compage
  pthread)


# Includes
target_include_directories(compage PUBLIC
  "${PROJECT_SOURCE_DIR}/compage/inc"
  "${PROJECT_SOURCE_DIR}/compage/lib"
)


# Build configurations
set(CMAKE_CONFIGURATION_TYPES Release Debug)

# Default build configuration
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()
