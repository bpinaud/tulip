QTX_SET_INCLUDES_AND_DEFINITIONS()
INCLUDE_DIRECTORIES(${TulipCoreBuildInclude} ${TulipCoreInclude} ${TulipOGLInclude} ${TulipGUIInclude} ${CMAKE_CURRENT_SOURCE_DIR})
FILE(GLOB LIB_SRCS "*.cpp")

ADD_LIBRARY(SVGExport-${TulipVersion} SHARED ${LIB_SRCS})
TARGET_LINK_LIBRARIES(SVGExport-${TulipVersion} ${LibTulipCoreName} ${LibTulipOGLName} ${QT_LIBRARIES})

TULIP_INSTALL_PLUGIN(SVGExport-${TulipVersion} ${TulipPluginsInstallDir})
