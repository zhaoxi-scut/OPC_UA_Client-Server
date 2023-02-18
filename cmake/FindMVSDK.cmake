set(MVSDK_ROOT_PATH "/opt/mindvision" CACHE PATH "the path to MindVision camera SDK")

# add the include directories path
list(APPEND MVSDK_INCLUDE_DIR "${MVSDK_ROOT_PATH}/include")
find_path(
    MVSDK_INCLUDE_DIR
    PATH "${MVSDK_INCLUDE_DIR}"
    NO_DEFAULT_PATH
)

# add libraries
find_library(
    MVSDK_LIB
    NAMES "libMVSDK.so"
    PATHS "${MVSDK_ROOT_PATH}/lib"
    NO_DEFAULT_PATH
)

mark_as_advanced(MVSDK_INCLUDE_DIR MVSDK_LIB)

set(MVSDK_INCLUDE_DIRS "${MVSDK_INCLUDE_DIR}")
set(MVSDK_LIBS "${MVSDK_LIB}")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    MVSDK
    REQUIRED_VARS MVSDK_LIB
)
