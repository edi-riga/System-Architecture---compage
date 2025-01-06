# Aggregate source files
AUX_SOURCE_DIRECTORY(src SOURCE_FILES)

# Add example's executable
add_executable(basic_usage ${SOURCE_FILES})

target_link_libraries(basic_usage
  compage_static)

# Includes
target_include_directories(basic_usage PUBLIC
  "${PROJECT_SOURCE_DIR}/examples/basic_usage"
  "${PROJECT_SOURCE_DIR}/compage/inc"
)
