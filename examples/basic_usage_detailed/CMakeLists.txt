# Aggregate source files
AUX_SOURCE_DIRECTORY(src SOURCE_FILES)

# RTCLM Client
add_executable(basic_usage_detailed ${SOURCE_FILES})

target_link_libraries(basic_usage_detailed
  compage_static)

# Includes
target_include_directories(basic_usage_detailed PUBLIC
  "${PROJECT_SOURCE_DIR}/examples/basic_usage_detailed"
  "${PROJECT_SOURCE_DIR}/compage/inc"
)
