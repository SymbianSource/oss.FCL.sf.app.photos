/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Thin callback wrapper
 *
*/




// include guard for the whole file content
#ifndef __SHWGEOMETRYUTILITIES_H__
#define __SHWGEOMETRYUTILITIES_H__

namespace NShwGeometryUtilities
    {
    /**
     * Utility to calculate the fitting for coordinates and maintaining the 
     * aspect ratio
     * @param aFitted reference to second coordinate, this value gets 
     *          multiplied with aNewAssigned / aAssigned
     * @param aAssigned reference to first coordinate, this value gets 
     *          assigned to aNewAssigned
     * @param aNewAssigned new value for first coordinate
     * 
     * Example: X coordinate needs to be assigned a new value X2 and you need
     * to calculate new Y coordinate maintaining the aspect ratio:
     * FitDimension( Y, X, X2 );
     * 
     * @author Kimmo Hoikka
     */
    template< typename T>
    void FitDimension( T& aFitted, T& aAssigned, T aNewAssigned )
        {
        // formula is:
        // aFitted = ( aFitted *  aNewAssigned ) / aAssigned
        // aAssigned = aNewAssigned
        // calculate second coordinate
        aFitted =
            ( aFitted * aNewAssigned ) /
                aAssigned;
        // assign the first coordinate
        aAssigned = aNewAssigned;
        }

    /**
     * Utility to fit width and height inside given box
     * maintaining the aspect ratio. Fit inside means that the image
     * does not fill the whole box and it is not stretched.
     * @param aWidth reference to the width to fit
     * @param aHeight reference to the height to fit
     * @param aBoxWidth the bounding box width
     * @param aBoxHeight the bounding box height
     */
    template< typename T>
    void FitInsideBox( T& aWidth, T& aHeight, T aBoxWidth, T aBoxHeight )
        {
        // calculate width difference
        T wdiff = aWidth - aBoxWidth;
        // calculate height difference
        T hdiff = aHeight - aBoxHeight;
        // check if wdiff is larger and positive
        if( ( wdiff > hdiff ) && wdiff > 0 )
            {
            // width is larger and too big for box so need to fit height
            FitDimension( aHeight, aWidth, aBoxWidth );
            }
        else if( hdiff > 0 )
            {
            // height is larger and too big for box so need to fit width
            FitDimension( aWidth, aHeight, aBoxHeight );
            }
        }

    /**
     * Utility to set width and height to cover given box
     * maintaining the aspect ratio. Fit cover means that the image
     * fills the whole box and some part of it does not fit inside the box.
     * @param aWidth reference to the width to fit
     * @param aHeight reference to the height to fit
     * @param aBoxWidth the bounding box width
     * @param aBoxHeight the bounding box height
     */
    template< typename T>
    void FitToCoverBox( T& aWidth, T& aHeight, T aBoxWidth, T aBoxHeight )
        {
        // try to fit with width first
        T newWidth = aWidth;
        T newHeight = aHeight;
        // calculate new height according to the width change
        FitDimension( newHeight, newWidth, aBoxWidth );
        // check if height is inside box
        if( newHeight < aBoxHeight )
            {
            // ok, it did not fill so fit according to height
            FitDimension( aWidth, aHeight, aBoxHeight );
            }
        else
            {
            // did fill the image so assign new values
            aWidth = newWidth;
            aHeight = newHeight;
            }
        }
    }
 

#endif // __SHWGEOMETRYUTILITIES_H__
