include_directories(
   ${DJVULIBRE_INCLUDE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../..
   ${CMAKE_BINARY_DIR}/okular
)


########### next target ###############

set(okularGenerator_djvu_SRCS
   generator_djvu.cpp
   kdjvu.cpp
)


okular_add_generator(okularGenerator_djvu ${okularGenerator_djvu_SRCS})

target_link_libraries(okularGenerator_djvu okularcore ${DJVULIBRE_LIBRARY} KF5::I18n)

########### install files ###############

install( PROGRAMS okularApplication_djvu.desktop org.kde.mobile.okular_djvu.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )


