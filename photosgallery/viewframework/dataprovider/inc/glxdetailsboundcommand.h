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
* Description:    Command Binding Class
*
*/




#ifndef _GLXDETAILSBOUNDCOMMAND_H_
#define _GLXDETAILSBOUNDCOMMAND_H_

#include <eikmenup.h>                   // helper class for extending CEikMenuPaneItem without breaking binary
#include <mglxmedialistprovider.h>      // Interface to allow a class that owns a media list to share access to it

class CGlxCommandHandlerAddToContainer; // Command handler for adding to a container
class CAknAppUi;
/** Error Id EMPY-7MKDHP **/ 
class MGlxMetadataDialogObserver ;		// call back to  details dailog

/**
 * MGlxBoundCommand
 */    
class MGlxBoundCommand
    {
public:
	/**
 	 * Destructor
	 */
    virtual ~MGlxBoundCommand() {};
    
   	/**
 	 * OfferCommandL
 	 * Handles user commands
 	 * @param aCommandId the command need to handle
	 */
    virtual TBool OfferCommandL( TInt aCommandId , MGlxMediaList& aMedialist) = 0;
    
   	/**
 	 * InitMenuL
 	 * Initialises a menu pane
 	 * @param aMenu representation of the menu pane
	 */
    virtual void InitMenuL( CEikMenuPane& aMenu ) const = 0;
    
    /**
     * HideLocationMenuItem
     * Hides Location Menu option
     * @param aMenu representation of the menu pane
     */
    virtual void HideLocationMenuItem( CEikMenuPane& aMenu ) const = 0;

    
    /**
    * Close the MGlxBoundCommand object.
    */ 
    virtual void Close() = 0;
    
    /**
     * Changes the Middle softkey command.
     */ 
    virtual void ChangeMskL() = 0;

    /**
     * Sets the Dailogs Observer
     */
    /** Error Id EMPY-7MKDHP **/ 
    virtual void SetDetailsDailogObserver( MGlxMetadataDialogObserver& aObserver ) = 0;

    };

/**
 * CGlxTitleBoundCommand
 */    
NONSHARABLE_CLASS( CGlxTitleBoundCommand ) : public CBase, public MGlxBoundCommand
	{
public:

	/**
 	 * Two phase construction
	 */
	IMPORT_C static CGlxTitleBoundCommand* NewL();

	/**
	 * Two phase construction
	 */
	static CGlxTitleBoundCommand* NewLC();
	
	/**
	* Destructor
	*/
	~CGlxTitleBoundCommand();
private:
	// From MGlxBoundCommand
	TBool OfferCommandL( TInt aCommandId ,MGlxMediaList& aMedialist);
	void InitMenuL( CEikMenuPane& aMenu ) const;
	void HideLocationMenuItem( CEikMenuPane& aMenu ) const;
	void Close();
	void ChangeMskL();
   	/**
    * SetDetailsDailogObserver
    * @param aObserver details dailog observer
    */
    /** Error Id EMPY-7MKDHP **/
	void SetDetailsDailogObserver( MGlxMetadataDialogObserver& aObserver );
    /**
    * ModifyTitleL
    * @param aData new title to be modified
    */  
    void ModifyTitleL(const TDesC& aData );

private://data

	MGlxMediaList* iMedialist;
	
	};
 
/**
 * CGlxTagsBoundCommand
 */	   	
NONSHARABLE_CLASS( CGlxTagsBoundCommand ) : public CBase, public MGlxBoundCommand, public MGlxMediaListProvider
	{
public:

	/**
	 * Two phase construction
	 */
	IMPORT_C static CGlxTagsBoundCommand* NewL();
	
	/**
	 * Two phase construction
	 */
	static CGlxTagsBoundCommand* NewLC();
	
	/**
	*ConstructL
	*/
	void ConstructL();
	
	/**
	* Destructor
	*/
	~CGlxTagsBoundCommand();
	
	/**
	*SetMediaList
	*@param aMedialist medialist
	*/
	void SetMediaList(MGlxMediaList* aMedialist);
	
	//From MGlxMediaListProvider
	 MGlxMediaList& MediaList();
private:
	// From MGlxBoundCommand
	TBool OfferCommandL( TInt aCommandId, MGlxMediaList& aMedialist);
	void InitMenuL( CEikMenuPane& aMenu ) const;
   void HideLocationMenuItem( CEikMenuPane& aMenu ) const;
	void Close();
	void ChangeMskL();
	/**
    * SetDetailsDailogObserver
    * @param aObserver details dailog observer
    */
    /** Error Id EMPY-7MKDHP **/
	void SetDetailsDailogObserver( MGlxMetadataDialogObserver& aObserver );
private:
	CGlxCommandHandlerAddToContainer* 	iAddToTag; //own
	MGlxMediaList* iMedialist;
	};	
 
/**
 * CGlxDescriptionBoundCommand
 */
NONSHARABLE_CLASS( CGlxDescriptionBoundCommand ) : public CBase, public MGlxBoundCommand
	{
public:

	/**
	 * Two phase construction
	 */
	IMPORT_C static CGlxDescriptionBoundCommand* NewL();
	
	/**
	 * Two phase construction
	 */
	static CGlxDescriptionBoundCommand* NewLC();
	
	/**
	* Destructor
	*/
	~CGlxDescriptionBoundCommand();
private:
	// From MGlxBoundCommand
	TBool OfferCommandL( TInt aCommandId , MGlxMediaList& aMedialist);
	void InitMenuL( CEikMenuPane& aMenu ) const;
    void HideLocationMenuItem( CEikMenuPane& aMenu ) const;	
	void Close();
	void ChangeMskL();
	/**
    * SetDetailsDailogObserver
    * @param aObserver details dailog observer
    */
    /** Error Id EMPY-7MKDHP **/
	void SetDetailsDailogObserver( MGlxMetadataDialogObserver& aObserver );
    /**
    * ModifyDescriptionL
    * @param aData new description to be modified
    */	
	void ModifyDescriptionL(const TDesC& aData );
	
private:
	MGlxMediaList* iMedialist;

	};
	
/**
 * CGlxLocationBoundCommand
 */
NONSHARABLE_CLASS( CGlxLocationBoundCommand ) : public CBase, public MGlxBoundCommand
	{
public:

	/**
	 * Two phase construction
	 */
	IMPORT_C static CGlxLocationBoundCommand* NewL();

	/**
	 * Two phase construction
	 */
	static CGlxLocationBoundCommand* NewLC();
	
	/**
	* Destructor
	*/
	~CGlxLocationBoundCommand();
private:
	// From MGlxBoundCommand
	TBool OfferCommandL( TInt aCommandId , MGlxMediaList& aMedialist);
	void InitMenuL( CEikMenuPane& aMenu ) const;
    void HideLocationMenuItem( CEikMenuPane& aMenu ) const;	
	void Close();
	void ChangeMskL();
	
    /**
    * RemoveLocationL
    * removes location information of an image
    */  
	void RemoveLocationL();	

    /**
    * SetDetailsDailogObserver
    * Sets detail dailog observer
    */
    /** Error Id EMPY-7MKDHP **/
	void SetDetailsDailogObserver(MGlxMetadataDialogObserver& aObserver) ;

private:
	MGlxMediaList* iMedialist;
	
    /// The AppUI. Not owned
    CAknAppUi* iAppUi;
    
    TBool iOnNaviKeySelect;

    MGlxMetadataDialogObserver*		iDialogObserver ;
	};

	
/**
 * CGlxAlbumsBoundCommand
 */
NONSHARABLE_CLASS( CGlxAlbumsBoundCommand ) : public CBase, public MGlxBoundCommand, public MGlxMediaListProvider
	{
public:

	/**
	 * Two phase construction
	 */
	IMPORT_C static CGlxAlbumsBoundCommand* NewL();
	
	/**
	 * Two phase construction
	 */
	static CGlxAlbumsBoundCommand* NewLC();
	
	/**
	*ConstructL
	*/
	void ConstructL();
	
	/**
	* Destructor
	*/
	~CGlxAlbumsBoundCommand();
	
	/**
	*SetMediaList
	*@param aMedialist medialist
	*/
	void SetMediaList(MGlxMediaList* aMedialist);
	
	//From MGlxMediaListProvider
 	MGlxMediaList& MediaList();

private:
	// From MGlxBoundCommand
	TBool OfferCommandL( TInt aCommandId, MGlxMediaList& aMedialist );
	void InitMenuL( CEikMenuPane& aMenu ) const;
    void HideLocationMenuItem( CEikMenuPane& aMenu ) const;	
	void Close();
	void ChangeMskL();
	/**
    * SetDetailsDailogObserver
    * @param aObserver details dailog observer
    */
	/** Error Id EMPY-7MKDHP **/
	void SetDetailsDailogObserver( MGlxMetadataDialogObserver& aObserver );
private:
	CGlxCommandHandlerAddToContainer* 	iAddToAlbum; //own
	MGlxMediaList* iMedialist;

	};
	
	
/**
 * CGlxResolutionBoundCommand
 */
NONSHARABLE_CLASS( CGlxResolutionBoundCommand ) : public CBase, public MGlxBoundCommand
	{
public:
	/**
	 * Two phase construction
	 */
	IMPORT_C static CGlxResolutionBoundCommand* NewL();

	/**
	 * Two phase construction
	 */
	static CGlxResolutionBoundCommand* NewLC();

	/**
	* Destructor
	*/
	~CGlxResolutionBoundCommand();
private:
	// From MGlxBoundCommand
	TBool OfferCommandL( TInt /*aCommandId*/ , MGlxMediaList& aMedialist);
	void InitMenuL( CEikMenuPane& /*aMenu*/ ) const;
    void HideLocationMenuItem( CEikMenuPane& aMenu ) const;	
	void Close();
	void ChangeMskL();
	/**
    * SetDetailsDailogObserver
    * @param aObserver details dailog observer
    */
	/** Error Id EMPY-7MKDHP **/
	void SetDetailsDailogObserver( MGlxMetadataDialogObserver& aObserver );
	};

/**
 * CGlxUsageRightsBoundCommand
 */
NONSHARABLE_CLASS( CGlxDurationBoundCommand ) : public CBase, public MGlxBoundCommand
	{
public:
	/**
	 * Two phase construction
	 */
	IMPORT_C static CGlxDurationBoundCommand* NewL();

	/**
	 * Two phase construction
	 */
	static CGlxDurationBoundCommand* NewLC();
	/**
	* Destructor
	*/
	~CGlxDurationBoundCommand();

private:
	// From MGlxBoundCommand
	TBool OfferCommandL( TInt /*aCommandId*/ , MGlxMediaList& aMedialist);
	void InitMenuL( CEikMenuPane& /*aMenu*/ ) const;
    void HideLocationMenuItem( CEikMenuPane& aMenu ) const;	
	void Close();
	void ChangeMskL();
	/**
    * SetDetailsDailogObserver
    * @param aObserver details dailog observer
    */
	/** Error Id EMPY-7MKDHP **/
	void SetDetailsDailogObserver( MGlxMetadataDialogObserver& aObserver );
	};


/**
 * CGlxUsageRightsBoundCommand
 */
NONSHARABLE_CLASS( CGlxUsageRightsBoundCommand ) : public CBase, public MGlxBoundCommand
	{
public:
	/**
	 * Two phase construction
	 */
	IMPORT_C static CGlxUsageRightsBoundCommand* NewL();

	/**
	 * Two phase construction
	 */
	static CGlxUsageRightsBoundCommand* NewLC();
	
	/**
	* SetMediaList
	* @param aMedialist medialist
	*/
	void SetMediaList(MGlxMediaList* aMedialist);
	
	/**
	* MediaList
	* @return MGlxMediaList main medialist
	*/
 	MGlxMediaList& MediaList();
 	
	/**
	* Destructor
	*/
 	~CGlxUsageRightsBoundCommand();
private:
	// From MGlxBoundCommand
	TBool OfferCommandL( TInt aCommandId , MGlxMediaList& aMedialist);
	void InitMenuL( CEikMenuPane& aMenu ) const;
    void HideLocationMenuItem( CEikMenuPane& aMenu ) const;	
	void Close();
	void ChangeMskL();
	/**
    * SetDetailsDailogObserver
    * @param aObserver details dailog observer
    */
	/** Error Id EMPY-7MKDHP **/
	void SetDetailsDailogObserver( MGlxMetadataDialogObserver& aObserver );
    /**
     * Launches the DRM view associated with the media.
     */
	void LaunchDRMViewL();
	
private:
	MGlxMediaList* iMedialist;

	};
	
/**
 * CGlxDateAndTimeBoundCommand
 */
NONSHARABLE_CLASS( CGlxDateAndTimeBoundCommand ) : public CBase, public MGlxBoundCommand
	{
public:
	/**
	 * Two phase construction
	 */
	IMPORT_C static CGlxDateAndTimeBoundCommand* NewL();

	/**
	 * Two phase construction
	 */
	static CGlxDateAndTimeBoundCommand* NewLC();

	/**
	* Destructor
	*/
	~CGlxDateAndTimeBoundCommand();
private:
	// From MGlxBoundCommand
	TBool OfferCommandL( TInt aCommandId , MGlxMediaList& aMedialist);
	void InitMenuL( CEikMenuPane& aMenu ) const;
    void HideLocationMenuItem( CEikMenuPane& aMenu ) const;	
	void Close();
	void ChangeMskL();
	/**
    * SetDetailsDailogObserver
    * @param aObserver details dailog observer
    */
	/** Error Id EMPY-7MKDHP **/
	void SetDetailsDailogObserver( MGlxMetadataDialogObserver& aObserver );
	};
	
/**
 * CGlxFileSizeBoundCommand
 */
NONSHARABLE_CLASS( CGlxFileSizeBoundCommand ) : public CBase, public MGlxBoundCommand
	{
public:
	/**
	 * Two phase construction
	 */
	IMPORT_C static CGlxFileSizeBoundCommand* NewL();

	/**
	 * Two phase construction
	 */
	static CGlxFileSizeBoundCommand* NewLC();

	/**
	* Destructor
	*/
	~CGlxFileSizeBoundCommand();
private:
	// From MGlxBoundCommand
	TBool OfferCommandL( TInt aCommandId , MGlxMediaList& aMedialist);
	void InitMenuL( CEikMenuPane& aMenu ) const;
    void HideLocationMenuItem( CEikMenuPane& aMenu ) const;	
	void Close();
	void ChangeMskL();
	/**
    * SetDetailsDailogObserver
    * @param aObserver details dailog observer
    */
	/** Error Id EMPY-7MKDHP **/
	void SetDetailsDailogObserver( MGlxMetadataDialogObserver& aObserver );
	};

#endif // _GLXDETAILSBOUNDCOMMAND_H_        
