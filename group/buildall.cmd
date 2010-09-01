rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:  Batch file to Builds all the photos components
rem

call bldmake bldfiles

REM build sources
call abld build winscw udeb
call abld build armv5 urel

REM build tests
REM call abld test build winscw udeb
REM call abld test build armv5 urel
