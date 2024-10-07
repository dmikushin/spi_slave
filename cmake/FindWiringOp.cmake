# Try to find the wiringOp library.
# This will define:
#
# WiringOp_FOUND       - wiringOp library is available
# WiringOp_INCLUDE_DIR - Where the wiringOp.h header file is
# WiringOp_LIBRARIES   - The libraries to link in.

find_library(WiringOp_LIBRARIES NAMES wiringOp PATHS
        /usr
        /usr/local
        /opt
        )

find_path(WiringOp_INCLUDE_DIR wiringOp.h PATHS
        /usr
        /usr/local
        /opt
        PATH_SUFFIXES
        )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WiringOp DEFAULT_MSG
        WiringOp_LIBRARIES
        WiringOp_INCLUDE_DIR
        )

mark_as_advanced(
        WiringOp_LIBRARIES
        WiringOp_INCLUDE_DIR
)
