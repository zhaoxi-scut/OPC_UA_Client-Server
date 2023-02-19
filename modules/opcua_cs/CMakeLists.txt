find_package(open62541 REQUIRED)
find_package(OpenCV REQUIRED)

asmpro_add_target(
    opcua_cs
    EXTERNAL open62541::open62541 ${OpenCV_LIBS}
    INSTALL
)
asmpro_set_properties(
    opcua_cs PROPERTIES
    CXX_STANDARD 17
)
asmpro_install_directories(
    include/asmpro
)
