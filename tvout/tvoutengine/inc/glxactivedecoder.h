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
* Description:    
*
*/


#ifndef GLXACTIVEDECODER_H_
#define GLXACTIVEDECODER_H_

// INCLUDES
#include <aknapp.h>
#include <imageconversion.h>
#include "glxhdmisurfaceupdater.h"

class CImageDecoder;
// CLASS DECLARATION

class CGlxHdmiDecoderAO : public CActive
    {
public:
    /*
     * NewL()
     * @Param1 CGlxHdmiSurfaceUpdater
     */
    static CGlxHdmiDecoderAO* NewL(CGlxHdmiSurfaceUpdater* aHdmiSurfaceUpdater);
    
    /*
     * Destructor
     */
    ~CGlxHdmiDecoderAO();
    
    /*
     * ConvertImageL() 
     * This calls the asyncronous service request to ICL convert
     * @param1 - Destination Bitmap
     * @param2 - Image decoder 
     */
    void ConvertImageL(CFbsBitmap& iBitmap,CImageDecoder* aDecoder);
    
protected:
    // from CActive
    void RunL();
    void DoCancel();
    
private:
    /*
     * Constructor
     */
    CGlxHdmiDecoderAO(CGlxHdmiSurfaceUpdater* aHdmiSurfaceUpdater);

private:
    CGlxHdmiSurfaceUpdater* iHdmiSurfaceUpdater; // not owned
    CImageDecoder* iDecoder;
    };

#endif /* GLXACTIVEDECODER_H_ */