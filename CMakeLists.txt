cmake_minimum_required(VERSION 3.22...3.28)

project(WRCC
        VERSION 2.0.0
        DESCRIPTION "Wheel-Rail Contact and Conicity Calculation"
        LANGUAGES CXX
)

# Qt Autogen settings
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Sanitizers)

option(BUILD_TESTING "Build the testing tree." ON)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Charts)

# Encapsulate precompiled shared libraries
set(VENDOR_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/libs")

foreach (LIB_NAME eqcon_lib conpon_lib trackplot_lib)
    add_library(${LIB_NAME} SHARED IMPORTED)
    set_target_properties(${LIB_NAME} PROPERTIES
            IMPORTED_LOCATION "${VENDOR_LIB_DIR}/lib${LIB_NAME}.so"
    )
endforeach ()

# -----------------------------------------------------------------------------
# Application Target
# -----------------------------------------------------------------------------
add_executable(${PROJECT_NAME}
        src/main.cpp
        src/core/input.cpp
        src/core/ReportExporter.cpp
        src/ui/widgets/LauncherWindow.cpp
        src/ui/widgets/BenchmarkWindow.cpp
        src/ui/widgets/CalculateWindow.cpp
        src/ui/widgets/customviewgroup.cpp
        src/ui/plots/BenchmarkPlots.cpp
        src/ui/plots/CalculatePlots.cpp

        # (AUTOMOC tracking)
        src/ui/widgets/LauncherWindow.h
        src/ui/widgets/BenchmarkWindow.h
        src/ui/widgets/CalculateWindow.h
        src/ui/widgets/customviewgroup.h
        src/ui/plots/BenchmarkPlots.h
        src/ui/plots/CalculatePlots.h
        src/ui/plots/customEvents.h
        include/wrcc/core/conicity.h
        include/wrcc/core/contact.h
        include/wrcc/core/pchip.h
        include/wrcc/models/input.h
        include/wrcc/services/ReportExporter.h
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/wrcc/core
        ${CMAKE_CURRENT_SOURCE_DIR}/include/wrcc/models
        ${CMAKE_CURRENT_SOURCE_DIR}/include/wrcc/services
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/plots
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Widgets
        Qt6::Gui
        Qt6::Charts
        eqcon_lib
        conpon_lib
        trackplot_lib
)

# -----------------------------------------------------------------------------
# Runtime & RPATH Configuration
# Ensures the binary can find the shared libraries without LD_LIBRARY_PATH
# -----------------------------------------------------------------------------
set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_RPATH "${VENDOR_LIB_DIR};$ORIGIN/../third_party/libs;$ORIGIN/third_party/libs"
        INSTALL_RPATH "$ORIGIN/../third_party/libs;$ORIGIN/third_party/libs"
        INSTALL_RPATH_USE_LINK_PATH TRUE
)

# -----------------------------------------------------------------------------
# Compiler Warnings & Sanitizers
# -----------------------------------------------------------------------------
wrcc_set_target_warnings(${PROJECT_NAME})
wrcc_enable_sanitizers(${PROJECT_NAME})

# -----------------------------------------------------------------------------
# Copy Data Files into Build Directory for Execution/Debugging
# -----------------------------------------------------------------------------
add_custom_target(copy_data_files ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
        COMMENT "Copying data files to output directory"
)
add_dependencies(${PROJECT_NAME} copy_data_files)

