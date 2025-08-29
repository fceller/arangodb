################################################################################
## FAISS
################################################################################

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${FAISS_EXE_LINKER_FLAGS}")

################################################################################
## global definitions
################################################################################

add_compile_warnings_flags()

if (USE_GOOGLE_TESTS)
  add_definitions("-DTEST_VIRTUAL=virtual")
else()
  add_definitions("-DTEST_VIRTUAL=")
endif()

if(MSVC)
  # MSVC: suppress warning C26812 (The enum type is unscoped. Prefer 'enum class' over 'enum')
  add_compile_options("/wd26812")
  # MSVC: suppress warning C4091 (unused typedef)
  add_compile_options("/wd4091")
  # MSVC: suppress warning C4100 (unreferenced formal parameter)
  add_compile_options("/wd4100") # suppress unused parameter warning
  # C4996: 'function': was declared deprecated
  add_compile_options("/wd4996")
  # C4217: 'function': nonstandard extension used : 'function' : conversion from 'type1' to 'type2'
  add_compile_options("/wd4217")
  # C4324 : 'function': structure was padded due to alignment specifier
  add_compile_options("/wd4324")
  # C4127: conditional expression is constant
  add_compile_options("/wd4127")
  # C4063: case 'value' is not a valid value for switch of enum 'type'
  add_compile_options("/wd4063")
endif()

# we can use clang switches with clang-cl on Windows as well
add_compile_options(-Wno-error=enum-constexpr-conversion -Wno-error=unused-local-typedef)

# add necessary defines for ICU. this will suffix all included ICU functions
# and the ICU namespace with _64_64
add_definitions("-DU_HAVE_LIB_SUFFIX=1")
add_definitions("-DU_LIB_SUFFIX_C_NAME=_64")

# also make sure that the correct ICU directory appears early in the list of include files
include_directories(BEFORE SYSTEM ${ICU64_INCLUDE_DIR})

if (MSVC)
add_definitions(-D_USE_MATH_DEFINES)
endif()

################################################################################
## arangod executable
################################################################################

if (MSVC AND NOT(SKIP_PACKAGING))
  generate_product_version(ProductVersionFiles
    NAME ${BIN_ARANGOD}
    FILE_DESCRIPTION ${ARANGODB_FRIENDLY_STRING}
    ICON ${ARANGO_ICON}
    COMPANY_NAME ${ARANGODB_PACKAGE_VENDOR}
    VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR}
    VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR}
    VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH}
    VERSION_REVISION ${BUILD_ID}
  )
endif ()

add_executable(${BIN_ARANGOD}
  RestServer/arangod.cpp
  ../cmake/activeCodePage.manifest
  ${ProductVersionFiles}
)
  
target_link_libraries(${BIN_ARANGOD}
  arangoserver
  arango_iresearch)

if (USE_JEMALLOC)
  target_link_libraries(${BIN_ARANGOD} ${JEMALLOC_LIB})
endif ()

add_dependencies(${BIN_ARANGOD} zlibstatic)
if (NOT USE_PRECOMPILED_V8)
  add_dependencies(${BIN_ARANGOD} v8_build)
endif ()
add_dependencies(${BIN_ARANGOD} iculegacy_build)
if (USE_JEMALLOC)
  add_dependencies(${BIN_ARANGOD} jemalloc_build)
endif ()
add_dependencies(${BIN_ARANGOD} man)

target_include_directories(${BIN_ARANGOD} PRIVATE
  "${PROJECT_SOURCE_DIR}/arangod"
  "${PROJECT_SOURCE_DIR}/${ENTERPRISE_INCLUDE_DIR}")

target_compile_features(${BIN_ARANGOD} PRIVATE cxx_constexpr)

################################################################################
## installation
################################################################################

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/")

install(
  TARGETS ${BIN_ARANGOD}
  RUNTIME DESTINATION ${CMAKE_INSTALL_SBINDIR}
)
install_config(arangod)

install_command_alias(${BIN_ARANGOD}
  ${CMAKE_INSTALL_SBINDIR}
  arango-secure-installation
)
install_config(arango-secure-installation)

install_command_alias(${BIN_ARANGOD}
  ${CMAKE_INSTALL_SBINDIR}
  arango-init-database
)
install_config(arango-init-database)

################################################################################
## libraries
################################################################################

get_target_property(IRESEARCH_INCLUDE
  iresearch-static INCLUDE_DIRECTORIES)

include(arangoserver.cmake)
add_subdirectory(Agency)
add_subdirectory(Aql)
add_subdirectory(Cache)
add_subdirectory(Cluster)
add_subdirectory(ClusterEngine)
add_subdirectory(Graph)
add_subdirectory(Indexes)
add_subdirectory(IResearch)
add_subdirectory(Metrics)
add_subdirectory(Network)
add_subdirectory(Replication)
add_subdirectory(Replication2)
add_subdirectory(RestHandler)
add_subdirectory(RestServer)
add_subdirectory(RocksDBEngine)
add_subdirectory(StorageEngine)
add_subdirectory(SystemMonitor)
add_subdirectory(Utils)
if (USE_V8)
  add_subdirectory(V8Server)
  target_link_libraries(${BIN_ARANGOD} arango_v8_no_deadline)
endif()
add_subdirectory(VocBase)
add_subdirectory(Zkd)
add_subdirectory(Scheduler)
