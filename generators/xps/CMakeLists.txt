include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}/../..
)


########### next target ###############

set(okularGenerator_xps_SRCS
   generator_xps.cpp
)

okular_add_generator(okularGenerator_xps ${okularGenerator_xps_SRCS})

target_link_libraries(okularGenerator_xps okularcore KF5::Archive KF5::I18n KF5::KIOCore Qt5::PrintSupport Qt5::Xml)

########### install files ###############

install( PROGRAMS okularApplication_xps.desktop org.kde.mobile.okular_xps.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )

