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
* Description:    Zoom Event Handler
*
*/


#ifndef MGLXZOOMEVENTHANDLERS_H_
#define MGLXZOOMEVENTHANDLERS_H_

#include <alf/alftimedvalue.h> // for TAlfRealPoint
#include <glxzoomview.hrh>
class MGlxZoomEventHandlers
    {
public: 
    /**
     * Handles the set view port
     */
    virtual void HandleViewPortParametersChanged(TPoint& aViewPortTopLeft ,
            TInt aTransitionTime ,
            TSize  *apViewPortDimension = NULL,
            TInt aPrimarySliderLevel = -1) = 0;  

    virtual void HandleShowUi(TBool aShow= EFalse) = 0;
    
    virtual void HandleZoomOutL(TInt aCommandId) = 0;
    
    virtual TBool HandlePointerEventsL(const TAlfEvent &aEvent)= 0;
    };
#endif /* MGLXZOOMEVENTHANDLERS_H_ */
