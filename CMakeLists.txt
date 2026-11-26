cmake_minimum_required(VERSION 3.16)
project(hearth-qt-style VERSION 0.1.0 LANGUAGES CXX)

# Build for one Qt major version per build directory: -DQT_MAJOR_VERSION=6 or 5.
set(QT_MAJOR_VERSION 6 CACHE STRING "Qt major version to build against (5 or 6)")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt${QT_MAJOR_VERSION} REQUIRED COMPONENTS Core Gui Widgets)

# Where the plugin goes: <prefix>/<Qt's plugin dir>/styles. Nix passes its
# qtPluginPrefix; elsewhere ask Qt.
if(NOT PLUGIN_INSTALL_DIR)
  if(QT_MAJOR_VERSION EQUAL 6)
    get_target_property(_qmake Qt6::qmake IMPORTED_LOCATION)
  else()
    get_target_property(_qmake Qt5::qmake IMPORTED_LOCATION)
  endif()
  execute_process(COMMAND ${_qmake} -query QT_INSTALL_PLUGINS OUTPUT_VARIABLE PLUGIN_INSTALL_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(PLUGIN_INSTALL_DIR "${PLUGIN_INSTALL_DIR}/styles")
endif()

add_library(hearthstyle MODULE src/hearthstyle.cpp src/hearthstyle.h src/plugin.cpp src/hearth.json)
target_link_libraries(hearthstyle PRIVATE Qt${QT_MAJOR_VERSION}::Core Qt${QT_MAJOR_VERSION}::Gui Qt${QT_MAJOR_VERSION}::Widgets)
set_target_properties(hearthstyle PROPERTIES OUTPUT_NAME hearth${QT_MAJOR_VERSION})
install(TARGETS hearthstyle LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR})

# A widget gallery for screenshots (test/gallery.cpp), installed as
# hearth-style-gallery-qt5 / -qt6.
option(BUILD_GALLERY "Build the widget gallery" ON)
if(BUILD_GALLERY)
  add_executable(gallery test/gallery.cpp)
  target_link_libraries(gallery PRIVATE Qt${QT_MAJOR_VERSION}::Core Qt${QT_MAJOR_VERSION}::Gui Qt${QT_MAJOR_VERSION}::Widgets)
  set_target_properties(gallery PROPERTIES OUTPUT_NAME hearth-style-gallery-qt${QT_MAJOR_VERSION})
  install(TARGETS gallery RUNTIME DESTINATION bin)
endif()

# Qt 6 only: the org.hearth.desktop Qt Quick Controls style (QML, our own
# components) and Kirigami's platform plugin for it.
if(QT_MAJOR_VERSION EQUAL 6)
  if(NOT QML_INSTALL_DIR)
    get_target_property(_qmake6 Qt6::qmake IMPORTED_LOCATION)
    execute_process(COMMAND ${_qmake6} -query QT_INSTALL_QML OUTPUT_VARIABLE QML_INSTALL_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
  endif()
  install(DIRECTORY qml/org DESTINATION ${QML_INSTALL_DIR})

  option(BUILD_KIRIGAMI_PLUGIN "Build Kirigami's platform plugin for the org.hearth.desktop style" ON)
  if(BUILD_KIRIGAMI_PLUGIN)
    find_package(KF6KirigamiPlatform REQUIRED)
    add_library(hearthkirigami MODULE kirigami/plugin.cpp kirigami/hearth-kirigami.json)
    target_link_libraries(hearthkirigami PRIVATE Qt6::Core Qt6::Qml KF6::KirigamiPlatform)
    find_package(Qt6 REQUIRED COMPONENTS Qml)
    # Kirigami finds it by the style's name: kf6/kirigami/platform/org.hearth.desktop.so
    set_target_properties(hearthkirigami PROPERTIES OUTPUT_NAME org.hearth.desktop PREFIX "")
    get_filename_component(_plugins_root "${PLUGIN_INSTALL_DIR}" DIRECTORY)
    install(TARGETS hearthkirigami LIBRARY DESTINATION ${_plugins_root}/kf6/kirigami/platform)
  endif()
endif()
