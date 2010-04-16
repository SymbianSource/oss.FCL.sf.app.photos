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
TEMPLATE = app
TARGET = photos
DEPENDPATH += .
INCLUDEPATH += . \
               ..\inc \
               ../ui/viewmanagement/statehandler/inc \
               ../loggers/loggerqt/inc \
               ../ui/inc \
               ../ui/uiengine/model/mediamodel/inc \
               ../ui/uiengine/medialistwrapper/inc \
               ../loggers/loggerqt/inc \
               ../ui/views/viewsfactory/inc \
               ../ui/views/viewbase/inc \
               ../ui/views/gridview/inc \
	       ../engine/collectionframework/plugins/glxcollectionpluginall/inc \
               ../traces

CONFIG += HB

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT


symbian: { 
    TARGET.UID3 = 0x20000A14
    ICON = ../data/qgn_menu_ovi_photos.svg
    isEmpty(TARGET.EPOCSTACKSIZE):TARGET.EPOCSTACKSIZE = 0x14000
    isEmpty(TARGET.EPOCHEAPSIZE):TARGET.EPOCHEAPSIZE = 20480 \
        41943040
    TARGET.CAPABILITY = ALL \
        -TCB
}

LIBS += -lglxstatehandler.dll \
        -lglxloggerqt.dll \
        -lglxviews.dll \
        -lglxmediamodel.dll

# Input
SOURCES += main.cpp \
	   glxfetcher.cpp 
HEADERS +=glxfetcher.h

RESOURCES += ../photos.qrc
TRANSLATIONS= photos.ts
###### qthighway
CONFIG += service
LIBS+=-lxqservice -lxqserviceutil -lflogger
SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable
##### /qthighway
