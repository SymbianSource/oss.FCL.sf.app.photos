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
INCLUDEPATH += ../../../commonutilities/imagedecoderwrapper/inc
INCLUDEPATH += /epoc32/include/mw/QtTest

CONFIG += qtestlib \
          Hb

LIBS += -lflogger.dll 
LIBS += -lglximagedecoderwrapper.dll

# Input
HEADERS += unittest_imagedecoderwrapper.h
          

SOURCES += unittest_imagedecoderwrapper.cpp
BLD_INF_RULES.prj_exports += "../../../gallery/data/Battle.jpg  /epoc32/winscw/c/data/images/Battle.jpg"