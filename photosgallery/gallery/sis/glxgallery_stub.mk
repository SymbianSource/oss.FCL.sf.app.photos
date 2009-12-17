#
# Copyright (c)  Nokia Corporation and/or its subsidiary(-ies).
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
# Description:   creates stub sis file
#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
    ZDIR = $(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
    ZDIR = $(EPOCROOT)epoc32\data\z
endif



do_nothing :
	@rem do_nothing
MAKMAKE : do_nothing
BLD : do_nothing
CLEAN : do_nothing
LIB : do_nothing
CLEANLIB : do_nothing

RESOURCE : do_nothing
FREEZE : do_nothing
SAVESPACE : do_nothing
RELEASABLES : do_nothing
FINAL : 
	echo =============================================================
	echo Starting to create stub file..
	makesis -v -s glxgallery_stub.pkg glxgallery_stub.sis
	echo Copying stub sis..
	copy glxgallery_stub.sis $(EPOCROOT)epoc32\data\Z\System\Install\glxgallery_stub.sis
	echo Stub created and copied.
	echo =============================================================
