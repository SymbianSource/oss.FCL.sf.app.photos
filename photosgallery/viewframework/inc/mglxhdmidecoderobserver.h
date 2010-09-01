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
* Description:    HDMI Image Decoder Observer
*
*/

#ifndef M_GLXHDMIDECODEROBSERVER_H
#define M_GLXHDMIDECODEROBSERVER_H

/** The HDMI Decoding status notifications */
enum THdmiDecodingStatus
    {
    EHdmiConnected = 0,
    EHdmiDecodingStarted, //in case Decoding started or HDMI is connected in b/w SlideShow 
    EHdmiDecodingFirstCompleted,
    EHdmiDecodingCompleted, //HDMI image decoding completed
    EHdmiDisconnected //in case HDMI is disconnected in b/w Slideshow
    };

/**
 * Class that observes HDMI Image Decoding status
 */
NONSHARABLE_CLASS(MGlxHDMIDecoderObserver)
    {
public:     
    /*
     * Handle notification of HDMI Image Decoder.
     */    
    virtual void HandleHDMIDecodingEventL(THdmiDecodingStatus aStatus) = 0;
    };


#endif // M_GLXHDMIDECODEROBSERVER_H
