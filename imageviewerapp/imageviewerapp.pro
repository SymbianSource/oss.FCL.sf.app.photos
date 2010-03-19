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

SERVICEAPP = app
TARGET = imageviewerapp
DEPENDPATH += .
INCLUDEPATH += . \
			   ../inc \
               ../ui/inc \
			   ../ui/viewmanagement/statehandler/inc \
               ../ui/uiengine/model/mediamodel/inc \
               ../ui/uiengine/medialistwrapper/inc \
               ../loggers/loggerqt/inc \
               ../ui/views/viewsfactory/inc \
               ../ui/views/viewbase/inc


CONFIG += hb

symbian: { 
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    TARGET.UID3 = 0x200071B3
    isEmpty(TARGET.EPOCSTACKSIZE):TARGET.EPOCSTACKSIZE = 0x14000
    isEmpty(TARGET.EPOCHEAPSIZE):TARGET.EPOCHEAPSIZE = 20480 \
        41943040
    TARGET.CAPABILITY = ALL \
        -TCB
}
RESOURCES += ../photos.qrc


LIBS += -lglxloggerqt.dll \
        -lglxviews.dll \
        -lglxmediamodel.dll \
		-lglxstatehandler.dll \
		-lglximageviewermanager.dll

# Input
SOURCES += main.cpp 

RSS_RULES += \
  "hidden = KAppIsHidden;" \
  "embeddability = KAppEmbeddableOnly;" \
  "datatype_list = " \
  "      {" \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/jpeg\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/jpg\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/jp2\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/tiff\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-wmf\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/ico\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/gif\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/bmp\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-bmp\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-bitmap\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-xbitmap\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-win-bitmap\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-windows-bmp\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/ms-bmp\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-ms-bmp\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/vnd.wap.wbmp\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/png\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-epoc-mbm\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/vnd.nokia.ota-bitmap\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/x-ota-bitmap\";" \  
  "          }," \
  "      DATATYPE" \
  "          {" \
  "          priority = EDataTypePriorityHigh;" \
  "          type = \"image/mng\";" \  
  "          }" \
  "      };" \

