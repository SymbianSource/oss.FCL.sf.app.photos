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
* Description:    Header of private wrapper class which speaks to HDMI and
*               also observes TVout connections
*
*/
#ifndef GLXTVOUTWRAPPERPRIVATE_P_H
#define GLXTVOUTWRAPPERPRIVATE_P_H

#include <QAbstractItemModel>
#include <fbs.h>
#include <QMetaType>

class GlxTvOutWrapper;
class CGlxConnectionObserver;
class CGlxHdmiController;

// Converting the QVariant to CFbsBitmap*
Q_DECLARE_METATYPE(CFbsBitmap*);

class GlxTvOutWrapperPrivate
    {
public:
    /*
     * Static method to create an instance
     */
    static GlxTvOutWrapperPrivate* Instance(GlxTvOutWrapper* aTvOutWrapper,
            QAbstractItemModel* aModel);

    /*
     * destructor
     */
    ~GlxTvOutWrapperPrivate();

    /*
     *  HandleConnectionChange
     *  @param1 true if connected, false if disconnected
     *  Note this is only being done for HDMI as there is no 
     *  implementation for Analog TV. 
     */
    void HandleConnectionChange(bool aConnected);
    
    /*
     * SetImagetoHDMI()
     */
    void SetImagetoHDMI();

    /*
     * Sets HDMI to cloning mode
     */
    void SetToCloningMode();
    
    /*
     * Sets HDMI to Native posting mode
     */
    void SetToNativeMode();
    
    /*
     * views should call this if for any item it doesnt want to move to 
     * HDMI posting mode
     */
    void ItemNotSupported();
    
    /*
     * Activate zoom in posting mode
     */
    void ActivateZoom(bool autoZoomOut);
    
    /*
     * Deactivate zoom in posting mode.
     */
    void DeactivateZoom();
private:
    /*
     * constructor
     */
    GlxTvOutWrapperPrivate(GlxTvOutWrapper* aTvOutWrapper,
            QAbstractItemModel* aModel);
    
    /*
     * constructL()
     */
    void ConstructL();
    
    /*
     * SetNewImage
     * Get the uri and bmp from the media model 
     * and pass it to HDMI controller
     */
    void SetNewImage();
private:
    GlxTvOutWrapper*            iTvOutWrapper;          // Instance of the Tv Wrapper
    QAbstractItemModel*         iModel;                 // Instance of the model
    CGlxConnectionObserver*     iConnectionObserver;    // Symbian connection observer
    CGlxHdmiController*         iHdmiController;        // Symbian Hdmi Controller
    
    bool       iHdmiConnected;                          // To check if the HDMI is connected
    bool       isImageSetToHdmi;                        // to check if the image is set to HDMI
    };

#endif //GLXTVOUTWRAPPERPRIVATE_P_H 

// End of file
