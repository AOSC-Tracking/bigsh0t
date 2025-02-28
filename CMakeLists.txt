cmake_minimum_required (VERSION 3.13)
project (bigsh0t)

set (CPP_SOURCE src/main/cpp)
set (CPP_TEST_SOURCE src/test/cpp)
set (COMMON_FILES
    ${CPP_SOURCE}/EMoR.cpp
    ${CPP_SOURCE}/Matrix.cpp
    ${CPP_SOURCE}/MPFilter.cpp
    ${CPP_SOURCE}/MPSource.cpp
    ${CPP_SOURCE}/Graphics.cpp
    ${CPP_SOURCE}/ImageProcessing.cpp
    ${CPP_SOURCE}/Math.cpp
    ${CPP_SOURCE}/MP4.cpp
    ${CPP_SOURCE}/SummedAreaTable.cpp
)
set (FREI0R_HOME src/frei0r)
set (PREPROCESSOR_COMMAND cat)
set (QML_POSTPROCESSOR_COMMAND python ${PROJECT_SOURCE_DIR}/format_qml.py)
set (DIST_NAME bigsh0t)
set (DIST_VERSION 2.6)
set (DIST_PLATFORM unknown)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "i686|x86|x86_64|AMD64")
    set (INTEL_ARCH ON)
endif()

if(APPLE)
    set (DIST_PLATFORM osx)
    include_directories("/usr/local/opt/libomp/include" /opt/local/include/libomp/)
    add_compile_options(-std=c++11 -Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include/ -I/opt/local/include/libomp/)
    if(INTEL_ARCH)
        add_compile_options(-msse2)
        add_compile_definitions(USE_SSE)
    endif()
    link_directories(/usr/local/opt/libomp/lib/ /opt/local/lib/libomp/)
    set (PREPROCESSOR_COMMAND cc -E -P -I${PROJECT_SOURCE_DIR}/src/main/shotcut/bigsh0t_transform_360/ - <)
endif()

if(MSVC)
    set (DIST_PLATFORM win)
    add_compile_options(/openmp)
    if(INTEL_ARCH)
        add_compile_definitions(USE_SSE)
    endif()
    set (PREPROCESSOR_COMMAND cl /EP)
    set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS_INIT} /DEF:${PROJECT_SOURCE_DIR}/${FREI0R_HOME}/msvc/frei0r_1_0.def")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    string(TOLOWER ${CMAKE_SYSTEM_NAME} DIST_PLATFORM)
    add_compile_options(-std=c++11 -fopenmp)
    if(INTEL_ARCH)
        add_compile_options(-msse2)
        add_compile_definitions(USE_SSE)
    endif()
    set (PREPROCESSOR_COMMAND gcc -E -P -I${PROJECT_SOURCE_DIR}/src/main/shotcut/bigsh0t_transform_360/ - <)
endif()

set (PACKAGE_NAME ${DIST_NAME}-${DIST_VERSION}-${DIST_PLATFORM})

macro (preprocess_shotcut_front_end plugin)
    add_custom_target(${plugin}_fe ALL)
    add_custom_command(
        TARGET ${plugin}_fe
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/shotcut/${plugin}
        COMMAND ${PREPROCESSOR_COMMAND} ${PROJECT_SOURCE_DIR}/src/main/shotcut/${plugin}/ui.qml > ${CMAKE_CURRENT_BINARY_DIR}/shotcut/${plugin}/ui.qml
        COMMAND ${QML_POSTPROCESSOR_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/shotcut/${plugin}/ui.qml
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/src/main/shotcut/${plugin}/meta.qml ${CMAKE_CURRENT_BINARY_DIR}/shotcut/${plugin}/meta.qml
        COMMENT "Preprocessing UI for ${plugin}"
        VERBATIM
    )
    if(EXISTS ${PROJECT_SOURCE_DIR}/src/main/shotcut/${plugin}/vui.qml)
        add_custom_command(
            TARGET ${plugin}_fe
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/shotcut/${plugin}
            COMMAND ${PREPROCESSOR_COMMAND} ${PROJECT_SOURCE_DIR}/src/main/shotcut/${plugin}/vui.qml > ${CMAKE_CURRENT_BINARY_DIR}/shotcut/${plugin}/vui.qml
            COMMAND ${QML_POSTPROCESSOR_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/shotcut/${plugin}/vui.qml
            COMMENT "Preprocessing ViewUI for ${plugin}"
            VERBATIM
        )
    endif()
endmacro(preprocess_shotcut_front_end)

macro (build_plugin plugin main_source)
    add_library(${plugin} MODULE ${CPP_SOURCE}/${main_source} ${COMMON_FILES})
    if(APPLE)
        target_link_options(${plugin} PUBLIC "-lomp")
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_options(${plugin} PUBLIC "-fopenmp")
    endif()
    set_target_properties(${plugin} PROPERTIES PREFIX "")
    preprocess_shotcut_front_end(${plugin})
    add_dependencies(create_tar ${plugin} ${plugin}_fe)
endmacro(build_plugin)

macro (build_test)
    add_executable(bigsh0t_test ${CPP_TEST_SOURCE}/main.cpp ${COMMON_FILES})
    if(APPLE)
        target_link_options(bigsh0t_test PUBLIC "-lomp")
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_options(bigsh0t_test PUBLIC "-fopenmp")
    endif()
    set_target_properties(bigsh0t_test PROPERTIES PREFIX "")
endmacro(build_test)


include_directories ("${PROJECT_SOURCE_DIR}/${FREI0R_HOME}/include")

add_custom_target(create_tar ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/shotcut/share/shotcut/qml/filters
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/shotcut/ ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/shotcut/share/shotcut/qml/filters
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:bigsh0t_hemi_to_eq> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:bigsh0t_eq_mask> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:bigsh0t_rect_to_eq> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:bigsh0t_transform_360> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:bigsh0t_eq_to_rect> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:bigsh0t_stabilize_360> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:bigsh0t_zenith_correction> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:bigsh0t_eq_to_stereo> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:bigsh0t_eq_cap> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:bigsh0t_eq_wrap> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1

    COMMAND ${CMAKE_COMMAND} -E tar "cfvz" "${PACKAGE_NAME}.tar.gz" ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}
    COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${PACKAGE_NAME}.zip" --format=zip ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}
)

build_plugin(bigsh0t_hemi_to_eq HemiToEq.cpp)
build_plugin(bigsh0t_eq_mask EqMask.cpp)
build_plugin(bigsh0t_transform_360 Transform360.cpp)
build_plugin(bigsh0t_rect_to_eq RectToEq.cpp)
build_plugin(bigsh0t_eq_to_rect EqToRect.cpp)
build_plugin(bigsh0t_stabilize_360 Stabilize360.cpp)
build_plugin(bigsh0t_zenith_correction ZenithCorrection.cpp)
build_plugin(bigsh0t_eq_to_stereo EqToStereo.cpp)
build_plugin(bigsh0t_eq_cap EqCap.cpp)
build_plugin(bigsh0t_eq_wrap EqWrap.cpp)
build_test()
