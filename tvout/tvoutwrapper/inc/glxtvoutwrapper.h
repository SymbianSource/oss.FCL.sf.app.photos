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
* Description:    Header of Wrapper class exposed to Views for HDMI
*
*/

#ifndef GLXTVOUTWRAPPER_H
#define GLXTVOUTWRAPPER_H

#include <QAbstractItemModel>

// forward decleration
class GlxTvOutWrapperPrivate;

#ifdef BUILD_TVOUTWRAPPER
#define GLX_TVOUTWRAPPER_EXPORT Q_DECL_EXPORT
#else
#define GLX_TVOUTWRAPPER_EXPORT Q_DECL_IMPORT
#endif

class GLX_TVOUTWRAPPER_EXPORT GlxTvOutWrapper
{
public:
    /*
     * Constructor
     */
    GlxTvOutWrapper();
    
    /*
     * Destructor
     */
    ~GlxTvOutWrapper();

    /*
     * Setting the model
     * This also creates the private instance of the wrapper
     * to pass the model 
     */
    void setModel(QAbstractItemModel* aModel);
    
    /*
     * Set the image on to HDMI
     */
    void setImagetoHDMI();
    
    /*
     * Sets HDMI to cloning mode
     */
    void setToCloningMode();
    
    /*
     * Sets HDMI to Native posting mode
     */
    void setToNativeMode();
    
    /*
     * views should call this if for any item it doesnt want to move to 
     * HDMI posting mode
     */
    void itemNotSupported();
    
    /*
     * Activate zoom in posting mode
     * This can be called if the zoom animation in HDMI(bounce back effect)
     * needs to be shown 
     * @param1 - send true if want to auto zoom out on animation effect
     */
    void activateZoom(bool autoZoomOut);
    
    /*
     * Deactivate zoom in posting mode.
     * This can be called when zoom animation needs to bounce back.
     * Note : This function is not required if bounce back effect finishes automatically
     */
    void deactivateZoom();
private:
    GlxTvOutWrapperPrivate* mTvOutWrapperPrivate;
};
    
#endif //GLXTVOUTWRAPPER_H 
// End of file
