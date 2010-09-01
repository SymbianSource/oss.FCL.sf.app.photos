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
rem Description:  Batch file to create sis file for udeb
rem

call makesis glxgallery_udeb.pkg
call signsis -s glxgallery_udeb.sis glxgallery_udeb.sisx RDTest_02.der RDTest_02.key