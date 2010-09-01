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
* Description:    Temporary rotation layout
*
*/




#include "glxrotationlayout.h" 

#include <alf/alfvisual.h>
#include <alf/alftimedvalue.h>
#include <alf/alftransformation.h>
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
EXPORT_C TGlxRotationLayout::TGlxRotationLayout()
    : iRotation( KGlxRotationNone )
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
EXPORT_C TGlxRotationLayout::~TGlxRotationLayout() 
    {
    }

// -----------------------------------------------------------------------------
// SetVisualListL
// -----------------------------------------------------------------------------
//
EXPORT_C void TGlxRotationLayout::SetRotation(TReal32 aRotation,
                                                TInt aTransitionTime)
    {
    // just remember the values
    iRotation = aRotation;
    iTransitionTime = aTransitionTime;
    }

// -----------------------------------------------------------------------------
// DoSetLayoutValues
// -----------------------------------------------------------------------------
void TGlxRotationLayout::DoSetLayoutValues(TGlxLayoutInfo& aInfo)
    {
    // get the visual
    CAlfVisual& visual = aInfo.Visual();
    TAlfTimedValue value( iRotation, iTransitionTime );
    
	// Update the transformation for the rotation
    CAlfTransformation& tranformation = visual.Transformation();
    tranformation.LoadIdentity();
    // rotate the visual
    tranformation.Rotate( value );
    }
