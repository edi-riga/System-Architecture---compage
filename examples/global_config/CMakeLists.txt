# Aggregate source files
AUX_SOURCE_DIRECTORY(src SOURCE_FILES)

# Add example's executable
add_executable(global_config ${SOURCE_FILES})

target_link_libraries(global_config
  compage_static)

# Includes
target_include_directories(global_config PUBLIC
  "${PROJECT_SOURCE_DIR}/examples/global_config"
  "${PROJECT_SOURCE_DIR}/compage/inc"
)
