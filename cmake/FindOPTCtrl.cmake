set(OPTCtrl_ROOT_PATH "/opt/OPT/OPTController" CACHE PATH "the path to OPT Machine Vision Controller SDK")

# add the include directories path
list(APPEND OPTCtrl_INCLUDE_DIR "${OPTCtrl_ROOT_PATH}/include")
find_path(
    OPTCtrl_INCLUDE_DIR
    PATH "${OPTCtrl_INCLUDE_DIR}"
    NO_DEFAULT_PATH
)

# add libraries
find_library(
    OPTCtrl_LIB
    NAMES "libOPTController.so"
    PATHS "${OPTCtrl_ROOT_PATH}/lib"
    NO_DEFAULT_PATH
)

mark_as_advanced(OPTCtrl_INCLUDE_DIR OPTCtrl_LIB)

set(OPTCtrl_INCLUDE_DIRS "${OPTCtrl_INCLUDE_DIR}")
set(OPTCtrl_LIBS "${OPTCtrl_LIB}")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    OPTCtrl
    REQUIRED_VARS OPTCtrl_LIB
)