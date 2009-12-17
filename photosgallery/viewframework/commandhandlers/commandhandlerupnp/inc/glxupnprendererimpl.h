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
* Description:    Implements rendering of the Image/Video
*
*/




#ifndef C_GLXUPNPRENDERERIMPL_H__
#define C_GLXUPNPRENDERERIMPL_H__

#include "glxupnprendererdefs.h"
#include "glxupnpthumbnailsaver.h"
   
//External Includes
#include <mglxactivemedialistchangeobserver.h>
#include <mglxactivemedialistresolver.h>
#include <mglxmedialistobserver.h>

//for upnp show command
#include <upnpshowcommand.h>
#include <upnpcommandobserver.h>


#include <glxmediaid.h>
#include <mglxmedialist.h> //for medialist

//Forward Declarations
class MGlxMediaList;
class CGlxDefaultAttributeContext;
class CGlxDefaultThumbnailContext;
class TGlxMediaId;
class CImageEncoder; 
class MGlxThumbnailSaveComplete;
class CGlxThumbnailSaver;

/**   
*  Implements the renderering of the image/video 
*
*  @lib glxupnpcommandhandler.lib   
*/

NONSHARABLE_CLASS( CGlxUpnpRendererImpl ):public CBase,
                                          public MGlxActiveMediaListChangeObserver,
					                      public MGlxMediaListObserver,
					                      public MUpnpCommandObserver, 
                                          public MGlxThumbnailSaveComplete
    {
public:
    
    /**
     * Two-phased constructor.
     *
     * @return Pointer
     */     
    static CGlxUpnpRendererImpl* NewL();
    
    /**
    * Destructor
    */    
    ~CGlxUpnpRendererImpl();  

private:

    /**
     * Symbian 2nd phase constructor
     */     
    void ConstructL();
    
    /**
    * Default Constructor
    */    
    CGlxUpnpRendererImpl();   
    
public:
    
    /**
    * Show the video file on the UPnP device
    */
    void UpnpShowVideoL();    

    /**
    * Access point is set or not
    * @return ETrue access point is defined
    */    
    static TBool IsSupported();
    
    /**
    * Error Status when Show Image/Video
    * @return TErrorStatus - UPnP Status
    */    
    NGlxUpnpRenderer::TErrorStatus UpnpErrorStatus();
    //From MGLxMediaListObserver 

    void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex,MGlxMediaList* aList);


    void HandleMediaL(TInt aListIndex, MGlxMediaList* aList);


    void HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex,MGlxMediaList* aList);


    void HandleItemModifiedL(const RArray<TInt>& aItemIndexes, MGlxMediaList* aList) ;


    void HandleAttributesAvailableL(TInt aItemIndex,const RArray<TMPXAttribute>& aAttributes,
                                    MGlxMediaList* aList);	


    void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, TInt aOldIndex,
                            MGlxMediaList* aList);

    void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList);

    void HandleMessageL(const CMPXMessage& aMessage, MGlxMediaList* aList);

    void HandleError(TInt aError) { (void)aError; };

    //From MGlxActiveMediaListChangeObserver

    void HandleActiveMediaListChanged();	   

    //From MUpnpCommandObserver

    void CommandComplete( TInt aStatusCode );   

    //MGlxThumbnailSaveComplete

    void HandleFileSaveCompleteL(const TDesC& aPath);

    // Callback function
    static TInt DoFocusChanged( TAny* aPtr );

private:
    /**
    * Creates the contextfor the medailist
    * @return none
    */
    void CreateContextL();    

    /**
    * Access the active media list of the view
    */
    void PrepareMediaListL(); 

    /**
    * Access the active media list of the view
    */        
    void SendFocusedItemL(TInt aNewIndex);
    
    /**
    * Access the active media list of the view
    */ 
    void SendFocusedItemL(TInt aNewIndex,const RArray<TMPXAttribute>& aAttributes);	
    
    /**
    * Get the image attribute
    */ 
    void GetImageL(const TGlxMedia& aMedia);
    
    /**
    * Get the video attribute
    */ 
    void GetVideoL(const TGlxMedia& aMedia,TMPXAttribute& aAttrThumbnail);
    
    /**
    * Show Command has been completed.
    */
    void CommandCompleteL( TInt aStatusCode );  
    
    /**
    * Display the Image after the Video play is Complete
    */ 
    void ShowImageAfterVideoL();

    /**
     * Used by the callback function to provide asynchronous focus change 
     */
    void DoFocusChangedL();
    
private:

    ///used to resolve the medialist from the views
    MGlxActiveMediaListResolver* iActiveMediaListResolver;   

    ///activemedialist obj , ref from the tile or full screen view
    MGlxMediaList* iActiveMediaList;     

    /// attribute context (owned)
    ///@ used for retrieving the uri attribute of the focused item(image/video)
    CGlxDefaultAttributeContext* iAttributeContext;     

    ///upnpshow command
    ///@owns CGlxUpnpRendererImpl
    CUpnpShowCommand* iUpnpShowCommand;
    
    //Check the previouusly focussed item
    TGlxMediaId iPreviouslySentMediaId;

    ///thumbnail context (owned)
    /// used for retrieving the thumbnail of the focused video from the tile views
    CGlxDefaultThumbnailContext* iThumbnailContext;
    
    ///Thumbnail saver class object(owned)
    ///@ used for saving the blited thumbnail image        
    CGlxThumbnailSaver* iThumbnailSaver;
    ///Flag to decide if video is to be shown on the rendering device
    //TRUE implies video is being shown on UPnP
    //FALSE implies image is being shown on UPnP
    TBool iIsVideoShow;
    
    //Store the path of the thumbnail video icon
    TBuf<35> iVideoThumbnail;
    //The UPnP Error Status when show is complete
    NGlxUpnpRenderer::TErrorStatus iUpnpErrorStatus;
    
    // Handle a change in focus asynchronously, owned
    CAsyncCallBack* iHandleFocusChangeCallback;
public:
	//flag to check if upnp is supported
	TBool iUpnpSupported;
    };					                      

#endif		