cmake_minimum_required(VERSION 3.21.1)

project(TableControl VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(VERBOSE "Architecture: ${ANDROID_TARGET_ARCH}")

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)


qt6_add_resources(SOURCES
        "qml.qrc"
)

qt_add_executable(appTableControl WIN32 MACOSX_BUNDLE
    main.cpp

    ${SOURCES}
    threadcontroller.h threadcontroller.cpp
    requestactions.h

    file/filemanager.h file/filemanager.cpp

    ui/uitransport.h ui/uitransport.cpp
    ui/playlistmodel.h ui/playlistmodel.cpp

    net/netclient.h net/netclient.cpp
    net/netmanager.h net/netmanager.cpp
    net/frames.h
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appTableControl PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appTableControl
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(appTableControl PUBLIC
    .
    file
    ui
    net
)

find_package(Qt6 REQUIRED COMPONENTS Gui Network Positioning Quick QuickControls2 LinguistTools Svg)
target_link_libraries(appTableControl PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Svg
    Qt6::Network
)

if(ANDROID)
    target_link_libraries(appTableControl PUBLIC
        Qt::CorePrivate)

    set_target_properties(appTableControl PROPERTIES
        QT_ANDROID_VERSION_NAME ${PROJECT_VERSION}
        QT_ANDROID_VERSION_CODE 1
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
    )

endif()



#qt6_add_translations(Pangaea-mobile TS_FILES
#    translations/pangaea-mobile_de.ts
#    translations/pangaea-mobile_en.ts
#    translations/pangaea-mobile_it.ts
#    translations/pangaea-mobile_ru.ts )

qt6_add_resources(appTableControl "AndroidResources"
    PREFIX
        "/"
    FILES
        android/AndroidManifest.xml
        android/build.gradle
)
