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




#include <AiwGenericParam.h>
#include "glxaiwmedia.h"
#include <glxassert.h>  // for GLX_ASSERT_DEBUG

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//	
CGlxAiwMedia::CGlxAiwMedia(const TGlxMediaId& aId)
: iId(aId)
    {
    
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//	
CGlxAiwMedia::~CGlxAiwMedia()
    {
    iParamArray.Close();
    }


// -----------------------------------------------------------------------------
// AddParamL
// -----------------------------------------------------------------------------
//	
TBool CGlxAiwMedia::AddParamL(const TAiwGenericParam& aParam)
    {
    if (KErrNotFound == iParamArray.Find(aParam))
        {
        iParamArray.AppendL(aParam);
        return ETrue;
        }
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// AppendToInParamsL
// -----------------------------------------------------------------------------
//	
void CGlxAiwMedia::AppendToInParamsL(CAiwGenericParamList& aInParams)
    {
    for (TInt index = 0; index < iParamArray.Count() ; index++ )
        {
        TAiwGenericParam& param = iParamArray[index];
        aInParams.AppendL(param);
        }
    iParamArray.Reset();
    }

// -----------------------------------------------------------------------------
// Id
// -----------------------------------------------------------------------------
//
const TGlxMediaId& CGlxAiwMedia::Id() const
    {
    return iId;
    }
