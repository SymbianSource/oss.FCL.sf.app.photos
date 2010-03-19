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
TARGET = glxcommoncommandhandlers
DEPENDPATH += . inc src

CONFIG += hb

INCLUDEPATH += . \
            ../commandhandlerbase/inc \
            ../../../commonutilities/common/inc \
            ../commandexecutionwrapper/inc \
            ../../uiengine/medialists/inc \
            ../../../inc \
            ../../inc \
            ../../uiengine/model/listmodel/inc \
            ../../uiengine/medialistwrapper/inc 

DEFINES += BUILD_COMMONCOMMANDHANDLERS

symbian: { 
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
TARGET.UID3 = 0x20000A0D
TARGET.CAPABILITY = ALL -TCB 
TARGET.EPOCALLOWDLLDATA = 1
}

LIBS += -lglxcommandhandlerbase.dll \
        -lmpxcollectionutility.dll \
        -lmpxcommon.dll \
        -lglxcommon.dll \
       	-lglxlistmodel.dll \
        -lglxmedialists.dll \
	-lexiflib.dll \
	-lfbscli.dll \
	-lbafl.dll

# Input
HEADERS += inc/glxcommandhandlerdelete.h
HEADERS += inc/glxcommandhandleraddtocontainer.h
HEADERS += inc/glxcommandhandlernewmedia.h
HEADERS += inc/glxcommandhandlerrotate.h

SOURCES += src/glxcommandhandlerdelete.cpp
SOURCES += src/glxcommandhandleraddtocontainer.cpp
SOURCES += src/glxcommandhandlernewmedia.cpp
SOURCES += src/glxcommandhandlerrotate.cpp

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

