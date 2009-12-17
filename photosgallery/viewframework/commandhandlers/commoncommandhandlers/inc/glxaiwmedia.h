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
* Description:    Photos AIW Media Contains AIW (InParam) information
*
*/




#ifndef _GLXAIWMEDIA_H
#define _GLXAIWMEDIA_H

/**
 * CGlxAiwMedia
 * Class to store AIW parameters for a given TGlxMediaId
 *
 * @author Rowland Cook
 */

#include <e32base.h>
#include "glxmediaid.h"


/**
 * Forward references
 */

class TAiwGenericParam;
class TGlxMediaId;

/**
 *  CGlxAiwMedia class
 *  This class contains an array AIW parameters for a given media item.
 */
NONSHARABLE_CLASS(CGlxAiwMedia) : public CBase
    {
public:
    /**
     *  Constructor
     *  @param TGlxMediaId of the Media Item
     */
    CGlxAiwMedia(const TGlxMediaId& aId);

    /**
     *  Destructor
     */
    virtual ~CGlxAiwMedia();
    
    /**
     *  AddParamL
     *  @param TAiwGenericParam& The AIW parameter that needs to be added to the
     *  array. This is only added if the paramter does not already exist.
     *  @return ETrue if added, EFalse if already exists
     */
    TBool AddParamL(const TAiwGenericParam& aParam);
    
    /**
     *  Id
     *  @return a reference to the TGlxMediaId of the item
     */
    const TGlxMediaId& Id() const;
    
    /**
     * Appends all the parameters in the iParamArray to aInParams.
     * @param aInParams reference to the InParams that is added too.
     */
     void AppendToInParamsL(CAiwGenericParamList& aInParams);

private:

    // The TGlxMediaId of the item associated with this class.
    TGlxMediaId                 iId;
    
    // The array of TAiwGenericParam that is associated with the TGlxMediaId (iId).
    RArray<TAiwGenericParam>    iParamArray;
    };

#endif
