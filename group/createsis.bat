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
rem Description:  Batch file to create sis file for photos application
rem

pushd .
cd ..\photosgallery\gallery\sis\
call makesis glxgallery.pkg
call signsis -s glxgallery.sis glxgallery.sisx RDTest_02.der RDTest_02.key
copy glxgallery.sisx ..\..\..\group\
popd