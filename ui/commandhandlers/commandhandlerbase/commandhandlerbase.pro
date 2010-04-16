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
TARGET = glxcommandhandlerbase
DEPENDPATH += . inc src

CONFIG += hb

INCLUDEPATH += . \
            ../../../commonutilities/common/inc \
            ../../../inc \
            ../../uiengine/medialists/inc \
            ../../inc \
            ../../../commonui/inc \
            ../../uiengine/medialistwrapper/inc \
            ../../uiengine/model/mediamodel/inc \
            ../../../traces	    

DEFINES += BUILD_COMMANDHANDLERBASE

LIBS += -lmpxcollectionutility.dll \
        -lmpxcommon.dll \
        -lglxcommon.dll \
        -lglxmedialists.dll\
        -lglxcommonui.dll \
        -lglxmediamodel.dll \
        -lcommonengine.dll

symbian: { 
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
TARGET.UID3 = 0x20000A11
TARGET.CAPABILITY = ALL -TCB
TARGET.EPOCALLOWDLLDATA = 1
}

# Input
HEADERS += inc/glxcommandhandler.h  inc/glxmpxcommandhandler.h inc/glxmodelcommandhandler.h
SOURCES += src/glxcommandhandler.cpp src/glxmpxcommandhandler.cpp src/glxmodelcommandhandler.cpp

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT
