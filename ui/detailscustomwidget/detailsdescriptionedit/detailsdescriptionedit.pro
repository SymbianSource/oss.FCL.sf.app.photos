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
TARGET = detailsdescriptionedit
CONFIG += hb
DEFINES += BUILD_DESCEDIT

win32{
CONFIG(release, debug|release){
  TARGET = detailsdescriptionedit
  DESTDIR = ../release # for easy plugin loading
}else{
  TARGET = detailsdescriptioneditd
  DESTDIR = ../debug # for easy plugin loading
  }
}

DEPENDPATH += ./inc \
              ./src

INCLUDEPATH += ./inc

SOURCES += \
		glxdetailsdescriptionedit.cpp

HEADERS += \
        glxdetailsdescriptionedit.h 
            
symbian {
    TARGET.UID3 = 0x200071B9
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    LIBS += -lbafl
}

defBlock = \      
"$${LITERAL_HASH}if defined(EABI)" \
"DEFFILE  ../eabi/detailsdescriptionedit.def" \
	 "$${LITERAL_HASH}else" \
	 "DEFFILE  ../bwins/detailsdescriptionedit.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock
# End of file	--Don't remove this
