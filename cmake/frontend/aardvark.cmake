################################################################################
## web interface build
################################################################################

find_package(Nodejs 20 REQUIRED)
find_package(Yarn REQUIRED)

set(FRONTEND_DESTINATION ${PROJECT_SOURCE_DIR}/js/apps/system/_admin/aardvark/APP/react)

<<<<<<< HEAD
option(NODE_MODULES_BUNDLE "specify pre-downloaded node modules for aardvark" "")


if (NODE_MODULES_BUNDLE AND EXISTS "${NODE_MODULES_BUNDLE}")
  add_custom_target(node_cache ALL
    COMMENT "extracting node modules"
    WORKING_DIRECTORY ${FRONTEND_DESTINATION}
    COMMAND ${CMAKE_COMMAND} -E tar xvJf "${NODE_MODULES_BUNDLE}"
    )
else()
  add_custom_target(node_cache ALL
    COMMENT "not extracting node modules"
    )
endif()

=======
if (WIN32)
  # On Windows cmake creates a cmd script for the custom command. However, yarn itself is
  # also a cmd script. We have to use the CALL command to run a nested batch script, because
  # otherwise the nested batch script will terminate the _parent_ script.
  set(YARN_PREFIX "CALL")
else ()
  set(YARN_PREFIX "")
endif ()
>>>>>>> parent of 6560fb090f1 (Remove Windows/MSVC support (#20636))

add_custom_target(frontend ALL
  DEPENDS node_cache
  COMMENT "create frontend build"
  WORKING_DIRECTORY ${FRONTEND_DESTINATION}
  COMMAND ${YARN_PREFIX} yarn install
  COMMAND ${YARN_PREFIX} yarn build
)

add_custom_target(frontend_clean
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${FRONTEND_DESTINATION}/build
  COMMENT "Removing frontend artifacts"
)
