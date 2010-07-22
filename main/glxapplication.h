/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   ?Description
*
*/

#ifndef GLXAPPLICATION_H_
#define GLXAPPLICATION_H_

#include <hbApplication.h>

class QSymbianEvent;

class GlxApplication: public HbApplication
{

public:

    GlxApplication( int &argc, char *argv[],
                        Hb::ApplicationFlags flags = Hb::DefaultApplicationFlags );
    
    GlxApplication( QApplication::QS60MainApplicationFactory factory,
                        int &argc, 
                        char *argv[],
                        Hb::ApplicationFlags flags = Hb::DefaultApplicationFlags );
    
    bool symbianEventFilter( const QSymbianEvent *aQSEvent );
    
private :
    void cleanUpCache();

};
    
#endif // GLXAPPLICATION_H_
