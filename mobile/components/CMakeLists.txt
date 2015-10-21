project(okular)

add_definitions(${QT_DEFINITIONS})
include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   
   ${QIMAGEBLITZ_INCLUDES}
)

set(okular_SRCS
    okularplugin.cpp
    ${CMAKE_SOURCE_DIR}/ui/guiutils.cpp
    ${CMAKE_SOURCE_DIR}/ui/tocmodel.cpp
    ${CMAKE_SOURCE_DIR}/ui/pagepainter.cpp
    pageitem.cpp
    documentitem.cpp
    thumbnailitem.cpp
    )

kconfig_add_kcfg_files(okular_SRCS ${CMAKE_SOURCE_DIR}/conf/settings.kcfgc )

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/settings.cpp PROPERTIES COMPILE_DEFINITIONS "okularpart_EXPORTS")

add_library(okularplugin SHARED ${okular_SRCS})
target_link_libraries(okularplugin
        Qt5::Quick
        Qt5::Qml
        Qt5::Core
        Qt5::Xml
        Qt5::Svg
        okularcore
        KF5::KDELibs4Support
        )

install(TARGETS okularplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/okular)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/okular)
install(FILES DocumentView.qml DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/okular)
install(FILES private/PageView.qml DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/okular/private)

#add_subdirectory(test)