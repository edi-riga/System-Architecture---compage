# Aggregate source files
AUX_SOURCE_DIRECTORY(src SOURCE_FILES)
AUX_SOURCE_DIRECTORY(lib/ini SOURCE_FILES)

# Set up library interface
add_library(compage INTERFACE)

# Set up separate targets for static/shared libraries (because .so must be compiled with PIC)
add_library(compage_static_object OBJECT ${SOURCE_FILES})
add_library(compage_shared_object OBJECT ${SOURCE_FILES})
add_library(compage_static STATIC $<TARGET_OBJECTS:compage_static_object>)
add_library(compage_shared SHARED $<TARGET_OBJECTS:compage_shared_object>)

# Properties
set_property(TARGET compage_shared_object PROPERTY POSITION_INDEPENDENT_CODE 1)
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

# Include routines for generating packageConfig file
include(CMakePackageConfigHelpers)

# Generate compageConfig.cmake file, later used by the find_package() routine
configure_package_config_file(
  CMakeConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/compageConfig.cmake"
  INSTALL_DESTINATION "lib/cmake/${PROJECT_NAME}"
)

# Install generated compageConfig.cmake file
install(
  FILES
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/compageConfig.cmake"
  DESTINATION
    lib/cmake/compage
  COMPONENT
    dev)


# Set up interface libraries
target_link_libraries(compage INTERFACE pthread)

# Set up interface include directories
target_include_directories(compage INTERFACE
  "${PROJECT_SOURCE_DIR}/compage/inc"
  "${PROJECT_SOURCE_DIR}/compage/lib"
)


# Add interface properties to all targets
target_link_libraries(compage_static compage)
target_link_libraries(compage_shared compage)
target_link_libraries(compage_static_object compage)
target_link_libraries(compage_shared_object compage)


# Build configurations
set(CMAKE_CONFIGURATION_TYPES Release Debug)

# Default build configuration
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()
