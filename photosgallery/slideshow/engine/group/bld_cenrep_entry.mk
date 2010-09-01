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
# Description:  make file
#

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : 
	erase %epocroot%epoc32\release\winscw\udeb\z\private\10202be9\200071d3.cre

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE :do_nothing
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES : do_nothing

FINAL : 
	echo Creating the Slideshow Engine Central Repository entries
	call %epocroot%epoc32\release\winscw\udeb\CentRepConv -nowait 200071d3.txt
	copy %epocroot%epoc32\winscw\c\200071d3.cre %epocroot%epoc32\release\winscw\udeb\z\private\10202be9\200071d3.cre
	erase %epocroot%epoc32\winscw\c\200071d3.cre
	echo Erased temporary files
	echo Finished creating the Slideshow Engine Central Repository entries