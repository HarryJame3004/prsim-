cmake_minimum_required(VERSION 3.21)

project(PrismDesktop VERSION 0.1.0 LANGUAGES CXX)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

include(CTest)
find_package(Qt6 6.4 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)
find_package(Qt6 6.4 REQUIRED COMPONENTS Concurrent Network)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GIO REQUIRED IMPORTED_TARGET gio-2.0 gio-unix-2.0)
if(BUILD_TESTING)
    find_package(Qt6 6.4 REQUIRED COMPONENTS Test)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
endif()

function(prism_warnings target)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    elseif(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    endif()
endfunction()

add_library(prism_core STATIC
    src/core/AppInfo.cpp src/core/AppInfo.h
    src/core/AppCatalog.cpp src/core/AppCatalog.h
    src/core/AppearanceSettings.cpp src/core/AppearanceSettings.h
)
target_include_directories(prism_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/core)
target_link_libraries(prism_core PUBLIC Qt6::Core Qt6::Gui PRIVATE PkgConfig::GIO)
target_compile_definitions(prism_core PRIVATE PRISM_VERSION="${PROJECT_VERSION}")
prism_warnings(prism_core)

add_library(prism_system STATIC
    src/system/SystemStatus.cpp src/system/SystemStatus.h
    src/system/AudioService.cpp src/system/AudioService.h
    src/system/BluetoothService.cpp src/system/BluetoothService.h
)
target_include_directories(prism_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/system)
target_link_libraries(prism_system PUBLIC Qt6::Core Qt6::Concurrent Qt6::Network)
prism_warnings(prism_system)

add_library(prism_layout STATIC
    src/window/LayoutPlanner.cpp src/window/LayoutPlanner.h
)
target_include_directories(prism_layout PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/window)
target_link_libraries(prism_layout PUBLIC Qt6::Core)
prism_warnings(prism_layout)

add_library(prism_displays STATIC
    src/displays/DisplayManager.cpp src/displays/DisplayManager.h
)
target_include_directories(prism_displays PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/displays)
target_link_libraries(prism_displays PUBLIC Qt6::Core Qt6::Gui)
prism_warnings(prism_displays)

function(prism_add_foundation_qml target)
    qt_add_resources(${target} foundation_qml
        PREFIX "/prism"
        BASE "${CMAKE_CURRENT_SOURCE_DIR}"
        FILES qml/FoundationWindow.qml
    )
endfunction()

set(PRISM_PREVIEW_QML
    qml/PreviewWindow.qml
    qml/GlassPanel.qml
    qml/TopBar.qml
    qml/FloatingDock.qml
    qml/GlassActionButton.qml
)

function(prism_add_preview_qml target)
    qt_add_resources(${target} preview_qml
        PREFIX "/prism"
        BASE "${CMAKE_CURRENT_SOURCE_DIR}"
        FILES ${PRISM_PREVIEW_QML}
    )
endfunction()

qt_add_executable(prism-foundation src/app/main.cpp)
target_link_libraries(prism-foundation PRIVATE prism_core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickControls2)
prism_add_foundation_qml(prism-foundation)
prism_warnings(prism-foundation)

qt_add_executable(prism-preview src/app/preview_main.cpp)
target_link_libraries(prism-preview PRIVATE prism_core prism_system prism_displays Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickControls2)
prism_add_preview_qml(prism-preview)
prism_warnings(prism-preview)

if(BUILD_TESTING)
    qt_add_executable(prism-tests tests/FoundationTests.cpp)
    target_link_libraries(prism-tests PRIVATE prism_core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickControls2 Qt6::Test)
    prism_add_foundation_qml(prism-tests)
    prism_warnings(prism-tests)
    add_test(NAME foundation COMMAND prism-tests)
    set_tests_properties(foundation PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

    qt_add_executable(prism-preview-tests tests/PreviewTests.cpp)
    target_link_libraries(prism-preview-tests PRIVATE prism_core prism_system prism_displays Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickControls2 Qt6::Test)
    prism_add_preview_qml(prism-preview-tests)
    prism_warnings(prism-preview-tests)
    add_test(NAME preview COMMAND prism-preview-tests)
    set_tests_properties(preview PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

    qt_add_executable(prism-catalog-tests tests/AppCatalogTests.cpp)
    target_link_libraries(prism-catalog-tests PRIVATE prism_core Qt6::Test)
    prism_warnings(prism-catalog-tests)
    add_test(NAME catalog COMMAND prism-catalog-tests)
    set_tests_properties(catalog PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

    qt_add_executable(prism-system-tests tests/SystemStatusTests.cpp)
    target_link_libraries(prism-system-tests PRIVATE prism_system Qt6::Test)
    prism_warnings(prism-system-tests)
    add_test(NAME system COMMAND prism-system-tests)

    qt_add_executable(prism-layout-tests tests/LayoutPlannerTests.cpp)
    target_link_libraries(prism-layout-tests PRIVATE prism_layout Qt6::Test)
    prism_warnings(prism-layout-tests)
    add_test(NAME layout COMMAND prism-layout-tests)

    qt_add_executable(prism-display-tests tests/DisplayManagerTests.cpp)
    target_link_libraries(prism-display-tests PRIVATE prism_displays Qt6::Test)
    prism_warnings(prism-display-tests)
    add_test(NAME displays COMMAND prism-display-tests)
    set_tests_properties(displays PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

    qt_add_executable(prism-audio-tests tests/AudioServiceTests.cpp)
    target_link_libraries(prism-audio-tests PRIVATE prism_system Qt6::Test)
    prism_warnings(prism-audio-tests)
    add_test(NAME audio COMMAND prism-audio-tests)

    qt_add_executable(prism-bluetooth-tests tests/BluetoothServiceTests.cpp)
    target_link_libraries(prism-bluetooth-tests PRIVATE prism_system Qt6::Test)
    prism_warnings(prism-bluetooth-tests)
    add_test(NAME bluetooth COMMAND prism-bluetooth-tests)

    add_test(NAME preview-performance
             COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/measure-preview.py
                     $<TARGET_FILE:prism-preview> --duration 4 --max-rss-mib 256
                     --max-cpu-percent 50)
    set_tests_properties(preview-performance PROPERTIES TIMEOUT 12)
endif()

install(TARGETS prism-preview RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES packaging/org.prismdesktop.Preview.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES LICENSE README.md DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/prism-desktop)

set(CPACK_PACKAGE_NAME "prism-desktop-preview")
set(CPACK_PACKAGE_VENDOR "Prism Desktop contributors")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Experimental Midnight Glass desktop preview")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_GENERATOR "TGZ")
include(CPack)
