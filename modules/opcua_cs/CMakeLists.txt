find_package(OpenCV REQUIRED)

asmpro_add_target(
    opcua_cs
    EXTERNAL open62541 pthread ${OpenCV_LIBS}
    INSTALL
)
asmpro_set_properties(
    opcua_cs PROPERTIES
    CXX_STANDARD 17
)
asmpro_install_directories(
    include/asmpro
)
