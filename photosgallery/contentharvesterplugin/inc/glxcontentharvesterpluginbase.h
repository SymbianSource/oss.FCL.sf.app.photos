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
* Description:    Base class for publishing all the content
*
*/




#ifndef GLXCONTENTHARVESTERPLUGINBASE_H
#define GLXCONTENTHARVESTERPLUGINBASE_H

//includes

#include <liwcommon.h>

//Forward Declarations

class CLiwGenericParamList;
class CLiwServiceHandler;
class MLiwInterface;
class CGlxContentHarvesterPluginBase;
class CGlxThumbnailContext;
class MGlxMediaList;
class MGlxContentHarvesterPlugin;

class MGlxContentHarvesterPluginEntry : public MLiwNotifyCallback
    {
public:
    /**
     * Virtual destructor
     */
    virtual ~MGlxContentHarvesterPluginEntry() {};
	virtual void ContainerCacheCleanupL(MGlxMediaList* aMediaList)=0;
    virtual void UpdatePreviewThumbnailListL()=0;
    virtual void RemoveContextAndObserver()=0;
    virtual TBool Focused()=0;
    virtual TInt Count()=0;
    };


class CGlxContentHarvesterPluginBase : public CBase, public MGlxContentHarvesterPluginEntry
    {
public: // Constructors and destructor

    /**
     * Default constructor.
     */
    CGlxContentHarvesterPluginBase( );
    
    /**
       * Perform the second phase construction 
       */     
    void ConstructL(TInt aDefaultIconId );

    /**
     * Destructor.
     */
    virtual ~CGlxContentHarvesterPluginBase();

    TSize GetGridIconSize();

    /* Calculate the Grid Icon Size for Grid Widget. It always calculate
     * size for Landscape orientation. This is done to avoid saving two
     * different thumbnail icons in Thumbnail database.
     */
    void SetGridIconSizeL();
 
    /**
     * Returns the focus status of this photossuite collection
     */
    TBool Focused();
	
    /*This returns the bitmap handle used to display the 
     * default thumbnail.
     */
    TInt GetBitmapHandle() const;


    /**
     * Updates  the focus value of the collection to true or false
     * @param aFoucs :True :  updates the collection is in focus
     *               :False : updates the collection is not in  focus  
     * 
     */
    void SetFocus(TBool aFocus );
    
    //To check if the matrix menu is in foreground or not
    TBool IsMatrixMenuInForegroundL();

    /* To register for Liw notifications 
     */
    void GetInterfaceForNotificationL();
    void RequestCpsNotificationL(TInt aSuiteItem);
    void SetupPublisherL(TInt aSuiteItem);
    void HandleStateChangeL(TInt aSuiteItem);

    /* Virtual APi which will be implemented in child classes 
     * Basically gives info if the item in suite has gained focus
     * or lost it
     */
    virtual void ActivateL(TBool aOn) = 0;

    /*
     * Creates Medialist based on the plugin id
     */
    MGlxMediaList* CreateMedialistAndAttributeContextL(const TGlxMediaId& aPluginId,
    		CGlxAttributeContext* aUriAttributeContext, 
    		CGlxAttributeContext* aThumbnailAttributeContext) const;

    /**
     * Method to fill input list for CPS add command
     */
    void FillInputListWithDataL(
            CLiwGenericParamList* aInParamList,
            const TDesC& aPublisherId, const TDesC& aContentType, 
            const TDesC& aContentId, TInt aHandle);

	/* Cleanup medialist cache */
    void ContainerCacheCleanupL(MGlxMediaList* aMediaList);
    
    /* Set content harvester plugin instance */
    void SetCHPlugin(MGlxContentHarvesterPlugin* aCHplugin);
    
    /* Get content harvester plugin instance */
    MGlxContentHarvesterPlugin* GetCHPlugin();
  
private: // data

    /**
     * Instance of CPS interface used for update with CPS.
     */
    TSize iGridIconSize;
    CLiwServiceHandler* iServiceHandler;
    CLiwGenericParamList* iInParamList;
    CLiwGenericParamList* iOutParamList;

    MLiwInterface* iCPSNotificationInterface;
    TGlxSequentialIterator iThumbnailIterator;    
    
    // Instance of content harvester plugin
    MGlxContentHarvesterPlugin* iCHplugin; 

    //This variable holds the value ,whether the collection is in focus or not
    TBool iIsFocused;
    
    //This variable is initialised with the default bitmap.
    CFbsBitmap* iBitmap;
    };
#endif /*GLXCONTENTHARVESTERPLUGINBASE_H*/
