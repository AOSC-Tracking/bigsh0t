cmake_minimum_required (VERSION 2.6)
project (f360)

set (CPP_SOURCE src/main/cpp)
set (COMMON_FILES ${CPP_SOURCE}/Matrix.cpp ${CPP_SOURCE}/MPFilter.cpp ${CPP_SOURCE}/MPSource.cpp ${CPP_SOURCE}/ImageProcessing.cpp)
set (FREI0R_HOME ../frei0r)
set (PREPROCESSOR_COMMAND cat)
set (DIST_NAME bigsh0t)
set (DIST_VERSION 1.0)
set (DIST_PLATFORM unknown)

if(APPLE)
    set (DIST_PLATFORM osx)
    add_compile_options(-Wc++11-extensions)
    set (PREPROCESSOR_COMMAND cc -E -P -I${PROJECT_SOURCE_DIR}/src/main/shotcut/transform_360/ - <)
endif()

if(MSVC)
    set (DIST_PLATFORM win)
    add_compile_options(/openmp)
    set (PREPROCESSOR_COMMAND cl /EP)
    set (CMAKE_MODULE_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS_INIT} " /DEF:${PROJECT_SOURCE_DIR}/${FREI0R_HOME}/msvc/frei0r_1_0.def")
endif()

set (PACKAGE_NAME ${DIST_NAME}-${DIST_VERSION}-${DIST_PLATFORM})

macro (preprocess_shotcut_front_end plugin)
    add_custom_target(${plugin}_fe ALL)
    add_custom_command(
        TARGET ${plugin}_fe        
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/shotcut/${plugin}
        COMMAND ${PREPROCESSOR_COMMAND} ${PROJECT_SOURCE_DIR}/src/main/shotcut/${plugin}/ui.qml > ${CMAKE_CURRENT_BINARY_DIR}/shotcut/${plugin}/ui.qml
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/src/main/shotcut/${plugin}/meta.qml ${CMAKE_CURRENT_BINARY_DIR}/shotcut/${plugin}/meta.qml
        COMMENT "Preprocessing UI for ${plugin}"
        VERBATIM
    )
endmacro(preprocess_shotcut_front_end)

macro (build_plugin plugin main_source)
    add_library(${plugin} MODULE ${CPP_SOURCE}/${main_source} ${COMMON_FILES})
    set_target_properties(${plugin} PROPERTIES PREFIX "")
    preprocess_shotcut_front_end(${plugin})
    add_dependencies(create_tar ${plugin} ${plugin}_fe)
endmacro(build_plugin)

include_directories ("${PROJECT_SOURCE_DIR}/${FREI0R_HOME}/include")

add_custom_target(create_tar ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/shotcut/share/shotcut/qml/filters
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/shotcut/ ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/shotcut/share/shotcut/qml/filters
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:hemi_to_eq> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:eq_mask> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:rect_to_eq> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:transform_360> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_NAME:eq_to_rect> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/lib/frei0r-1
    
    COMMAND ${CMAKE_COMMAND} -E tar "cfvz" "${PACKAGE_NAME}.tar.gz" ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}
)    

build_plugin(hemi_to_eq HemiToEq.cpp)
build_plugin(eq_mask EqMask.cpp)
build_plugin(transform_360 Transform360.cpp)
build_plugin(rect_to_eq RectToEq.cpp)
build_plugin(eq_to_rect EqToRect.cpp)
