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
TARGET = detailsnamelabel
CONFIG += hb
DEFINES += BUILD_NAMELABEL

win32{
CONFIG(release, debug|release){
  TARGET = detailsnamelabel
  DESTDIR = ../release # for easy plugin loading
}else{
  TARGET = detailsnamelabeld
  DESTDIR = ../debug # for easy plugin loading
  }
}

DEPENDPATH += ./inc \
              ./src

INCLUDEPATH += ./inc

SOURCES += \
		glxdetailsnamelabel.cpp

HEADERS += \
        glxdetailsnamelabel.h 
            
symbian {
    TARGET.UID3 = 0x2000A7BC
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    LIBS += -lbafl
}

defBlock = \      
"$${LITERAL_HASH}if defined(EABI)" \
"DEFFILE  ../eabi/detailsnamelabel.def" \
	 "$${LITERAL_HASH}else" \
	 "DEFFILE  ../bwins/detailsnamelabel.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

# End of file	--Don't remove this
