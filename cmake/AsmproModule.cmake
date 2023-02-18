set(Asmpro_MODULES_PUBLIC "" CACHE INTERNAL "List of Asmpro public modules marked for export")
set(Asmpro_MODULES_INTERFACE "" CACHE INTERNAL "List of Asmpro interface modules marked for export")

# ----------------------------------------------------------------------------
#   将指定路径下的所有文件安装至特定目标
#   用法:
#       asmpro_install_directories(<directory> [DST_LIB])
#   示例:
#       asmpro_install_directories(include/asmpro)
# ----------------------------------------------------------------------------
function(asmpro_install_directories _dir)
    cmake_parse_arguments(IS "" "DESTINATION" "" ${ARGN})
    # Search
    set(current_dir "${CMAKE_CURRENT_LIST_DIR}/${_dir}")
    execute_process(COMMAND ls ${current_dir} OUTPUT_VARIABLE subs)
    string(REPLACE "\n" ";" subs "${subs}")
    foreach(sub ${subs})
        if(IS_DIRECTORY ${current_dir}/${sub})
            set(dirs ${dirs} "${current_dir}/${sub}")
        else()
            set(files ${files} "${current_dir}/${sub}")
        endif()
    endforeach(sub ${subs})
    
    # Install
    if(IS_DESTINATION)
        set(install_dir ${IS_DESTINATION})
    else()
        set(install_dir ${Asmpro_INSTALL_PATH}/${_dir})
    endif()
    if(dirs)
        install(DIRECTORY ${dirs} DESTINATION ${install_dir})
    endif()
    if(files)
        install(FILES ${files} DESTINATION ${install_dir})
    endif()
endfunction(asmpro_install_directories _dirs)

# ----------------------------------------------------------------------------
#   在当前目录中添加新的 Asmpro 目标
#   用法:
#       asmpro_add_target(<name> [SHARED] [EXTRA_HEADER] <list of 3rd party include directories>]
#           [DEPENDS <list of asmpro dependencies>] [EXTERNAL <list of 3rd party dependencies>] [INSTALL])
#   示例:
#       asmpro_add_target(
#           xxxxx_detector                      # 需要生成的目标 (文件夹名)
#           EXTRA_HEADER ${OpenCV_INCLUDE_DIRS} # 依赖的第三方头文件目录
#           DEPENDS xxxxx                       # 依赖的 Asmpro 目标库 (文件夹名)
#           EXTERNAL ${OpenCV_LIBS}             # 依赖的第三方目标库
#           INSTALL                             # 将目标库安装至特定目录的标志
#       )
# ----------------------------------------------------------------------------
function(asmpro_add_target _name)
    # Add module options
    set(options INSTALL)
    set(multy_args DEPENDS EXTRA_HEADER EXTERNAL)
    cmake_parse_arguments(ADD "${options}" "" "${multy_args}" ${ARGN})

    # Add library
    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/src/${_name})
        set(module_dir ${CMAKE_CURRENT_LIST_DIR}/src/${_name})
    else()
        set(module_dir ${CMAKE_CURRENT_LIST_DIR}/src)
    endif()
    aux_source_directory(${module_dir} target_src)

    # Bind parameter object
    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/src/${_name}/para)
        set(para_dir ${CMAKE_CURRENT_LIST_DIR}/src/${_name}/para)
    else()
        set(para_dir ${CMAKE_CURRENT_LIST_DIR}/src/para)
    endif()
    unset(para_src)
    if(IS_DIRECTORY ${para_dir})
        aux_source_directory(${para_dir} para_src)
    endif()

    if(BUILD_SHARED_LIBS)
        add_library(asmpro_${_name} SHARED ${target_src} ${para_src})
    else()
        add_library(asmpro_${_name} STATIC ${target_src} ${para_src})
    endif()

    # Add dependence
    target_include_directories(
        asmpro_${_name}
        PUBLIC ${ADD_EXTRA_HEADER}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    foreach(_dep ${ADD_DEPENDS})
        target_link_libraries(
            asmpro_${_name}
            PUBLIC asmpro_${_dep} asmpro_para
        )
    endforeach(_dep ${ADD_DEPENDS})
    target_link_libraries(
        asmpro_${_name}
        PUBLIC ${ADD_EXTERNAL}
    )

    # Install
    if(ADD_INSTALL)
        install(
            TARGETS asmpro_${_name}
            EXPORT AsmproTargets
            LIBRARY DESTINATION lib
        )
        set(
            Asmpro_MODULES_PUBLIC ${Asmpro_MODULES_PUBLIC} "asmpro_${_name}"
            CACHE INTERNAL "List of Asmpro public modules marked for export"
            FORCE
        )
    endif(ADD_INSTALL)
endfunction(asmpro_add_target _name)

# ----------------------------------------------------------------------------
#   在当前目录中添加新的 Asmpro 接口库
#   用法:
#       asmpro_add_interface(<name> [EXTRA_HEADER <list of 3rd party include directories>]
#           [DEPENDS <list of asmpro dependencies>] [EXTERNAL <list of 3rd party dependencies>] [INSTALL])
#   示例:
#       asmpro_add_interface(
#           detector                            # 需要生成的目标 (文件夹名)
#           EXTRA_HEADER ${OpenCV_INCLUDE_DIRS} # 依赖的第三方头文件目录
#           DEPENDS combo                       # 依赖的 Asmpro 目标库 (文件夹名)
#           EXTERNAL ${OpenCV_LIBS}             # 依赖的第三方目标库
#           INSTALL                             # 将目标库安装至特定目录的标志
#     )
# ----------------------------------------------------------------------------
function(asmpro_add_interface _name)
    # Add interface target options
    set(options INSTALL)
    set(multy_args EXTRA_HEADER DEPENDS EXTERNAL)
    cmake_parse_arguments(ADDINT "${options}" "" "${multy_args}" ${ARGN})

    # Add library
    add_library(asmpro_${_name} INTERFACE)
    target_include_directories(
        asmpro_${_name}
        INTERFACE ${ADDINT_EXTRA_HEADER}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    foreach(_dep ${ADDINT_DEPENDS})
        target_link_libraries(
            asmpro_${_name}
            INTERFACE asmpro_${_dep}
        )
    endforeach(_dep ${ADDINT_DEPENDS})
    target_link_libraries(
        asmpro_${_name}
        INTERFACE ${ADD_EXTERNAL}
    )
    # Install
    if(ADDINT_INSTALL)
        install(
            TARGETS asmpro_${_name}
            EXPORT AsmproTargets
        )
        set(
            Asmpro_MODULES_INTERFACE ${Asmpro_MODULES_INTERFACE} "asmpro_${_name}"
            CACHE INTERNAL "List of Asmpro interface modules marked for export"
            FORCE
        )
    endif(ADDINT_INSTALL)
endfunction(asmpro_add_interface _name)

# ----------------------------------------------------------------------------
#   将预处理定义添加至指定目标
#   用法:
#   asmpro_compile_definitions(<target>
#       <INTERFACE | PUBLIC | PRIVATE> [items1...]
#       [<INTERFACE | PUBLIC | PRIVATE> [items2...] ...])
#   示例:
#   asmpro_compile_definitions(
#       feature
#       INTERFACE Armor_LIB
#   )
# ----------------------------------------------------------------------------
macro(asmpro_compile_definitions _target)
    target_compile_definitions(
        asmpro_${_target}
        ${ARGN}
    )
endmacro(asmpro_compile_definitions _target)

# ----------------------------------------------------------------------------
#   此命令用于为指定模块添加新的 Asmpro 测试
#   用法:
#   asmpro_add_test(<name> <Unit|Performance> <DEPENDS> [asmpro_target...]
#       <DEPEND_TESTS> [test_target...])
#   示例:
#   asmpro_add_test(
#       detector Unit                  # 测试名
#       DEPENDS xxxxx_detector         # 需要依赖的 Asmpro 目标库
#       DEPEND_TESTS GTest::gtest_main # 需要依赖的第三方测试工具目标库
#   )
# ----------------------------------------------------------------------------
function(asmpro_add_test test_name test_kind)
    # Add arguments variable
    set(multy_args DEPENDS DEPEND_TESTS)
    if(NOT "${test_kind}" MATCHES "^(Unit|Performance)$")
        message(FATAL_ERROR "Unknown test kind : ${test_kind}")
    endif()
    cmake_parse_arguments(TS "" "" "${multy_args}" ${ARGN})
    string(TOLOWER "${test_kind}" test_kind_lower)
    set(test_report_dir "${CMAKE_BINARY_DIR}/test-reports/${test_kind_lower}")
    file(MAKE_DIRECTORY "${test_report_dir}")

    # Add testing executable
    if("${test_kind_lower}" STREQUAL "performance")
        set(test_suffix "perf_test")
        set(test_dir "perf")
    else()
        set(test_suffix "test")
        set(test_dir "test")
    endif()
    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/${test_dir}/${test_name})
        set(test_dir ${CMAKE_CURRENT_LIST_DIR}/${test_dir}/${test_name})
    else()
        set(test_dir ${CMAKE_CURRENT_LIST_DIR}/${test_dir})
    endif()
    aux_source_directory(${test_dir} test_src)
    set(the_target asmpro_${test_name}_${test_suffix})
    add_executable(${the_target} ${test_src})

    # Add depends
    foreach(_dep ${TS_DEPENDS})
        target_link_libraries(
            ${the_target}
            PRIVATE asmpro_${_dep}
        )
    endforeach(_dep ${TS_DEPENDS})

    # Test depends
    target_link_libraries(
        ${the_target}
        PRIVATE ${TS_DEPEND_TESTS}
    )
    if("${test_kind_lower}" STREQUAL "unit")
        gtest_discover_tests(
            ${the_target}
            WORKING_DIRECTORY "${test_report_dir}"
            EXTRA_ARGS "--gtest_output=xml:${test_name}-report.xml"
        )
    else()
        add_test(
            NAME "${the_target}"
            WORKING_DIRECTORY "${test_report_dir}"
            COMMAND "${the_target}" --benchmark_out=${test_name}-perf-report.txt
        )
    endif()
endfunction(asmpro_add_test test_name test_kind)

# ----------------------------------------------------------------------------
#   设置如何构建指定 Target 的属性
#   用法:
#   asmpro_set_properties(target1 target2 ...
#                       PROPERTIES prop1 value1
#                       prop2 value2 ...)
#   示例:
#   asmpro_set_properties(
#       detector       # 目标名
#       PROPERTIES CXX_STANDARD 17                    
#   )
# ----------------------------------------------------------------------------
macro(asmpro_set_properties _target)
    set_target_properties(
        asmpro_${_target} ${ARGN}
    )
endmacro(asmpro_set_properties _target)