#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#

TEMPLATE = app
TARGET = 
DEPENDPATH += . 
              
INCLUDEPATH += . 
INCLUDEPATH += ../../../ui/uiengine/medialistwrapper/inc
INCLUDEPATH += ../../../ui/uiengine/medialists/inc
INCLUDEPATH += ../../../ui/inc
INCLUDEPATH += ../../../inc
INCLUDEPATH += /epoc32/include/mw/QtTest

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
               
CONFIG += qtestlib \
          Hb

LIBS += -lflogger.dll 
LIBS += -lglxmedialistwrapper.dll

# Input
HEADERS += unittest_medialistwrapper.h
          

SOURCES += unittest_medialistwrapper.cpp

symbian: { 
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCHEAPSIZE = 0x20000 0x1600000
    TARGET.UID3 = 0x20000A0E
    MMP_RULES += SMPSAFE 
}