# Aggregate source files
AUX_SOURCE_DIRECTORY(src SOURCE_FILES)

# RTCLM Client
add_executable(controller_startup ${SOURCE_FILES})

target_link_libraries(controller_startup
  compage_static)

# Includes
target_include_directories(controller_startup PUBLIC
  "${PROJECT_SOURCE_DIR}/examples/controller_startup"
  "${PROJECT_SOURCE_DIR}/compage/inc"
)
