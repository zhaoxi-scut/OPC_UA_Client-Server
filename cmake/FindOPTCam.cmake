set(OPTCam_ROOT_PATH "/opt/OPT/OPTCameraDemo" CACHE PATH "the path to OPT Machine Vision Camera SDK")

# add the include directories path
list(APPEND OPTCam_INCLUDE_DIR "${OPTCam_ROOT_PATH}/include")
find_path(
    OPTCam_INCLUDE_DIR
    PATH "${OPTCam_INCLUDE_DIR}"
    NO_DEFAULT_PATH
)

# add libraries
find_library(
    OPTCam_LIB
    NAMES "libOPTSDK.so"
    PATHS "${OPTCam_ROOT_PATH}/lib"
    NO_DEFAULT_PATH
)

mark_as_advanced(OPTCam_INCLUDE_DIR OPTCam_LIB)

set(OPTCam_INCLUDE_DIRS "${OPTCam_INCLUDE_DIR}")
set(OPTCam_LIBS "${OPTCam_LIB}")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    OPTCam
    REQUIRED_VARS OPTCam_LIB
)