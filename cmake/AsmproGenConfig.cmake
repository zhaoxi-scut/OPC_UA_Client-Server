# --------------------------------------------------------------------------------------------
#  Installation for CMake Module:  AsmproConfig.cmake
# --------------------------------------------------------------------------------------------

set(Asmpro_MODULES_CONFIGCMAKE ${Asmpro_MODULES_PUBLIC})
set(Asmpro_INCLUDE_DIRS_CONFIGCMAKE ${Asmpro_INSTALL_PATH}/include)
file(RELATIVE_PATH Asmpro_INSTALL_PATH_RELATIVE_CONFIGCMAKE 
    "${CMAKE_INSTALL_PREFIX}/${Asmpro_CONFIG_INSTALL_PATH}/" ${CMAKE_INSTALL_PREFIX})

# make install
set(CONFIG_BUILD_DIR "${CMAKE_BINARY_DIR}/config-install")
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/templates/AsmproConfig.cmake.in"
    "${CONFIG_BUILD_DIR}/AsmproConfig.cmake"
    @ONLY
)

# 导出普通公开目标、接口目标
export(
    TARGETS ${Asmpro_MODULES_PUBLIC} ${Asmpro_MODULES_INTERFACE}
    FILE ${CMAKE_BINARY_DIR}/AsmproTargets.cmake
)
# 
install(
    EXPORT AsmproTargets
    FILE AsmproTargets.cmake
    DESTINATION "${Asmpro_CONFIG_INSTALL_PATH}"
)

install(
    FILES "${CONFIG_BUILD_DIR}/AsmproConfig.cmake"
    DESTINATION "${Asmpro_CONFIG_INSTALL_PATH}"
)
