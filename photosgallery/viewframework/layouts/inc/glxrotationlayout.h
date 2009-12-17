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




#ifndef GLXROTATIONLAYOUT_H
#define GLXROTATIONLAYOUT_H

#include <glxlayout.h>

const TReal32 KGlxRotationNone = 0.0;
const TReal32 KGlxRotationClockwise = -90.0;
const TReal32 KGlxRotationAnticlockwise = 90.0;

/**
 *  Apply animated rotation to a visual.
 *
 *  @lib glxlayouts.lib 
 */
 
class TGlxRotationLayout : public TGlxLayout
    {
    public:
        /**
         * Constructor, reset the values
         */
        IMPORT_C TGlxRotationLayout();

        /**
         * Destructor
         */
        IMPORT_C ~TGlxRotationLayout();

        /**
         * Set rotation value.
         * @param aRotation New rotation value, in degrees.
         * @param aTransitionTime How long the change in rotation should take.
         */
        IMPORT_C void SetRotation(TReal32 aRotation, TInt aTransitionTime);

    protected:	// From TGlxLayout
        void DoSetLayoutValues(TGlxLayoutInfo& aInfo);

    private:

        /** Amount of rotation, in degrees */
        TReal32 iRotation;
        /** Varies between 0 and 1 to animate a change in rotation over time */
        TInt iTransitionTime;
    };

#endif  // GLXROTATIONLAYOUT_H
