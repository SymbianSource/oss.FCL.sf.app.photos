/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Symbian application class
*
*/




#ifndef C_VIEWERAPPLICATION_H
#define C_VIEWERAPPLICATION_H

#include <aknapp.h>

/**
 *  CGlxApplication
 *
 *  @lib ViewerApplication
 */
class CGlxApplication : public CAknApplication
    {
public:  // from CAknApplication
    TUid AppDllUid() const;

protected: // from CAknApplication
    CApaDocument* CreateDocumentL();
    };

#endif // C_VIEWERAPPLICATION_H
