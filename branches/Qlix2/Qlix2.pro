###########################:###########################################
# Automatically generated by qmake (2.01a) Sat Jan 19 15:54:06 2008
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += . mtp widgets
INCLUDEPATH += . mtp widgets
CONFIG += debug

# Input
HEADERS += linuxsignals.h \
           types.h \
           mtp/Icon.h \
           mtp/BmpStructs.h \
           mtp/MtpDevice.h \
           mtp/MtpObject.h \
           mtp/MtpSubSystem.h \
           widgets/QlixPreferences.h \
           widgets/DeviceButton.h \
           widgets/MtpWatchDog.h \ 
           widgets/QMtpDevice.h \ 
           widgets/AlbumModel.h \
           widgets/DirModel.h \
           widgets/PlaylistModel.h \
           widgets/DeviceChooser.h \
           widgets/DeviceExplorer.h \
           widgets/QlixMainWindow.h
SOURCES += main.cpp \
           mtp/MtpDevice.cpp \
           mtp/MtpObject.cpp \
           mtp/MtpSubSystem.cpp \
           widgets/QlixPreferences.cpp \
           widgets/DeviceButton.cpp \
           widgets/QMtpDevice.cpp \ 
           widgets/MtpWatchDog.cpp \
           widgets/AlbumModel.cpp \
           widgets/DirModel.cpp \
           widgets/PlaylistModel.cpp \ 
           widgets/DeviceChooser.cpp \
           widgets/DeviceExplorer.cpp \
           widgets/QlixMainWindow.cpp
RESOURCES += Qlix.qrc
unix {
  DEFINES += LINUX_SIGNALS
  DEFINES += QLIX_DEBUG

  TAGLIB_INC = $$system(taglib-config --cflags)
  TAGLIB_LIBS = $$system(taglib-config --libs)

  LIBS += $$TAGLIB_LIBS
  LIBS += "-lmtp"

  INCLUDEPATH +="/usr/include/"
  INCLUDEPATH +="/usr/local/include/"
  DEPENDPATH += "/usr/local/lib/"
  QMAKE_CXXFLAGS += $$TAGLIB_INC
}
include(modeltest/modeltest.pri)
