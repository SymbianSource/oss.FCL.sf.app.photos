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
* Description:    Implementation of private wrapper class
*
*/

#include "glxtvoutwrapper.h"
#include "glxtvoutwrapper_p.h"
#include "glxtvconnectionobserver.h"
#include "glxhdmicontroller.h"
#include "glxmodelparm.h"
#include "glxviewids.h"

// -----------------------------------------------------------------------------
// Static method to create the private wrapper instance 
// -----------------------------------------------------------------------------
GlxTvOutWrapperPrivate* GlxTvOutWrapperPrivate::Instance(GlxTvOutWrapper* aTvOutWrapper,
        QAbstractItemModel* aModel)
    {
    GlxTvOutWrapperPrivate* self = new GlxTvOutWrapperPrivate(aTvOutWrapper,aModel);
    if (self){
        TRAPD(err,self->ConstructL());
        if(err != KErrNone){
            delete self;
            self = NULL;
            }
        }
    return self;
    }

// -----------------------------------------------------------------------------
// ConstructL
// This creates the Connection observer and the Hdmi Controller
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::ConstructL()
    {
    iConnectionObserver = CGlxConnectionObserver::NewL(this);
    if (!iHdmiController) {
        iHdmiController = CGlxHdmiController::NewL();
        iHdmiConnected = iHdmiController->IsHDMIConnected();
        }
    }
    
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
GlxTvOutWrapperPrivate::GlxTvOutWrapperPrivate(GlxTvOutWrapper* aTvOutWrapper,
        QAbstractItemModel* aModel):iTvOutWrapper(aTvOutWrapper),
                iModel(aModel),
                iConnectionObserver(NULL),
                iHdmiController(NULL),
                iHdmiConnected(false),
                isImageSetToHdmi(false)
    {
    // Do Nothing
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
GlxTvOutWrapperPrivate::~GlxTvOutWrapperPrivate()
    {
    if (iConnectionObserver){
    delete iConnectionObserver;
    iConnectionObserver = NULL;
    }
    if (iHdmiController){
    delete iHdmiController;
    iHdmiController = NULL;
    }
    }

// -----------------------------------------------------------------------------
// HandleConnectionChange
// observes the connection change to modify the connection flag
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::HandleConnectionChange(bool aConnected)
    {
    iHdmiConnected = aConnected;
    // if Connection state positive and uri/bmp are not passed to HDMI already
    // then it is a new image - Set it.
    if (!isImageSetToHdmi && iHdmiConnected && getSubState() !=IMAGEVIEWER_S)
        {
        SetNewImage();
        }
    }

// -----------------------------------------------------------------------------
// SetImagetoHDMI
// if the Hdmi is connected, Set the new image else do nothing
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::SetImagetoHDMI()
    {
    if (iHdmiConnected)
        {
        // Set the Image
        SetNewImage();
        // Set the flag after HDMI is set for the image
        isImageSetToHdmi = true;    
        }
    else
        {
        // UnSet the flag as HDMI not set
        // this would be in future use if HDMI not set for a image in FS/SS and
        // cable connected
        isImageSetToHdmi = false;
        }
    }

// -----------------------------------------------------------------------------
// SetNewImage
// Private implementation of setting the image to HDMI
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::SetNewImage()
    {
    QVariant focusVariant =(iModel->data(iModel->index(0,0),GlxFocusIndexRole)); 
    int focusIndex;
    if (focusVariant.isValid() && focusVariant.canConvert<int>()) {
        focusIndex = (focusVariant.value<int>());
	}
	else{
		return ;
	}
    
    // Get the image uri
    QString imagePath = (iModel->data(iModel->index(focusIndex,0),GlxUriRole)).value<QString>();
    if(imagePath.isNull()) {
    // Null path no need to proceed
        return ;
    }
   
    TPtrC aPtr = reinterpret_cast<const TUint16*>(imagePath.utf16());
    QVariant var = (iModel->data(iModel->index(focusIndex,0),GlxHdmiBitmap));
    CFbsBitmap* bmp = var.value<CFbsBitmap*>();
    iHdmiController->SetImageL(aPtr,bmp);
    }

// -----------------------------------------------------------------------------
// setToCloningMode 
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::SetToCloningMode()
    {
    if(iHdmiController){
    iHdmiController->ShiftToCloningMode();
    }
    }

// -----------------------------------------------------------------------------
// setToNativeMode 
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::SetToNativeMode()
    {
    if(iHdmiController){
    iHdmiController->ShiftToPostingMode();
    }
    }

// -----------------------------------------------------------------------------
// itemNotSupported 
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::ItemNotSupported()
    {
    if(iHdmiController){
    iHdmiController->ItemNotSupported();
    }
    }

// -----------------------------------------------------------------------------
// activateZoom 
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::ActivateZoom(bool autoZoomOut)
    {
    if(iHdmiController){
    iHdmiController->ActivateZoom(autoZoomOut);
    }
    }

// -----------------------------------------------------------------------------
// deactivateZoom 
// -----------------------------------------------------------------------------
void GlxTvOutWrapperPrivate::DeactivateZoom()
    {
    if(iHdmiController){
    iHdmiController->DeactivateZoom();
    }
    }

// -----------------------------------------------------------------------------
// getSubState 
// -----------------------------------------------------------------------------
int GlxTvOutWrapperPrivate::getSubState()
{
    int substate = NO_FULLSCREEN_S;
    QVariant variant = iModel->data( iModel->index(0,0), GlxSubStateRole );    
    if ( variant.isValid() &&  variant.canConvert<int> ()  ) {
        substate = variant.value<int>();
    }
    return substate;
}

// End of file
