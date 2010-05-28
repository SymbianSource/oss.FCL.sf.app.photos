#/* 
#* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
#* All rights reserved.
#* This component and the accompanying materials are made available
#* under the terms of "Eclipse Public License v1.0"
#* which accompanies this distribution, and is available
#* at the URL "http://www.eclipse.org/legal/epl-v10.html".
#*
#* Initial Contributors:
#* Nokia Corporation - initial contribution.
#*
#* Contributors:
#* 
#* Description:
#*
#*/ 
TEMPLATE = lib
TARGET = glxviews

DEPENDPATH += . fullscreenview/inc fullscreenview/src gridview/inc gridview/src slideshowsettingsview/inc slideshowsettingsview/src viewbase/inc
CONFIG += hb

LIBS += -lglxfavmediamodel.dll \
        -lglximagedecoderwrapper.dll \
        -lglxloggerqt.dll \
        -lshareui.dll \
        -lglxtvout.dll \
        -lglxtvoutwrapper.dll \
        -lxqserviceutil.dll \
        -lglxlogging.dll \
        -lganeswidgets.dll \
        -lglxmodelwrapper.dll \
        -lglxviewutilities.dll \
        -lglxzoomwidget.dll

DEFINES += BUILD_GLXVIEWS

INCLUDEPATH += . ../inc \
          ../../inc \
	  ../widgets/glxzoomwidget/inc \
          ../uiengine/model/favmediamodel/inc \
          ../uiengine/medialistwrapper/inc \
          ../viewbase/inc \
          ../../commonutilities/imagedecoderwrapper/inc \
          ../../loggers/loggerqt/inc \
          ../../traces \
          ../../tvout/tvoutwrapper/inc \
          ../uiengine/model/modelwrapper/inc \
          ../viewutilities/effectengine/inc \
          ../viewutilities/settingutility/inc \
          /epoc32/include/mw/hgwidgets

symbian: { 
TARGET.UID3 = 0x200009EF
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
TARGET.EPOCALLOWDLLDATA = 1
TARGET.CAPABILITY = ALL -TCB 
}

# Input
HEADERS += viewbase/inc/glxview.h \
           fullscreenview/inc/glxcoverflow.h \
           fullscreenview/inc/glxfullscreenview.h \
           detailsview/inc/glxdetailsview.h  \
           detailsview/inc/glxdetailscustomwidgets.h  \
           detailsview/inc/glxdetailscustomicon.h \
           gridview/inc/glxgridview.h \
           slideshowsettingsview/inc/glxslideshowsettingsview.h \
           listview/inc/glxlistview.h \
           slideshowview/inc/glxslideshowview.h \
           slideshowview/inc/glxslideshowwidget.h \
           docloaders/inc/glxviewdocloader.h \
           viewsfactory/inc/glxviewsfactory.h \
           
SOURCES += viewbase/src/glxview.cpp \
           fullscreenview/src/glxcoverflow.cpp \
           fullscreenview/src/glxfullscreenview.cpp \
	   detailsview/src/glxdetailsview.cpp \
           detailsview/src/glxdetailscustomwidgets.cpp \
           detailsview/src/glxdetailscustomicon.cpp \
           gridview/src/glxgridview.cpp \
           slideshowsettingsview/src/glxslideshowsettingsview.cpp \
           listview/src/glxlistview.cpp \
           slideshowview/src/glxslideshowview.cpp \
           slideshowview/src/glxslideshowwidget.cpp \
	   docloaders/src/glxviewdocloader.cpp \
           viewsfactory/src/glxviewsfactory.cpp

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT 