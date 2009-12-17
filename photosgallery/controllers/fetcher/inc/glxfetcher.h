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
* Description:    Header class for fetcher application plugin
*
*/




#ifndef GLXFETCHER_H_
#define GLXFETCHER_H_


// INCLUDES
#include <badesca.h>
#include <mediafetchimplementation.h>
#include <MMGFetchCanceler.h>			// MMGFetchCanceler

#include <glxfilterfactory.h>         	// CMPXFilter

// CLASS FWD DECLARATION
class CGlxFetcherDialog;
// CLASS DECLARATION
NONSHARABLE_CLASS( CGlxFetcher ) : public CMediaFetchImplementation,
                                   public MMGFetchCanceler 
                                                                                  														
	{
public: //constructor and desctructor
	/**
     * Two-phased constructor.
     *
     * @since 3.2
     * @return Pointer to newly created object.
     */
	static CGlxFetcher* NewL();
		
    /**
     * Destructor.
     */
	virtual ~CGlxFetcher();
		
protected:	// from CMediaFetchImplementation
    /**
     * By default fetcher is single selection fetcher.
     * If multi-selection is enabled then plug-in should display a
     * multi-selection fetcher and allow user to select several items.
     * @param aMultiSelect if ETrue then multiselection fetcher is enabled
     */
	void SetMultiSelectionL( TBool aMultiSelect );

    /**
     * Not in requirement
     * @param aMimeTypes plug-in should show only files with given Mime types
     */
    void SetMimeTypesL( const MDesCArray& aMimeTypes );

    /**
     * Plug-in should use given text as selection key (SK1) label
     * @param aSelectionSoftkey Custom selection softkey label
     */
    void SetSelectionSoftkeyL( const TDesC& aSelectionSoftkey );

    /**
     * Plug-in should use given text as heading
     * @param aTitle Custom title text for fetcher
     */
    void SetHeadingL( const TDesC& aTitle );

    /**
     * Plug-in should call this interface before selection is done.
     * if MMGFetchVerifier::VerifySelectionL() returns ETrue
     * then the user's selection should be accepted and the fetcher
     * closed. If EFalse is returned then user should change the current
     * selection before it can be accepted and the fetcher closed.
     * @param aVerifier See MMGFetchVerifier.h
     */
    void SetVerifierL( MMGFetchVerifier& aVerifier );

    /**
     * Plug-in should return canceler interface.
     * @param aCanceler See MMGFetchCanceler.h
     */
	void GetCancelerL( MMGFetchCanceler*& aCanceler );

    /**
     * Plug-in should launch fetcher popup for given media type.
     * @param aSelectedFiles plug-in appends selected files to this array
     * @param aMediaType plug-in should display only files with this media type
     * return ETrue if user selected file(s), EFalse if use cancelled.
     */
    TBool LaunchL( CDesCArray& aSelectedFiles, TMediaFileType aMediaType );
    

private : //constructor and desctructor  	
    /**
     * C++ default constructor.
     */
	CGlxFetcher();
		 
    /**
     * By default Symbian 2nd phase constructor is private.
     */
	void ConstructL();

    /** @return view title when fetcher is opened for selecting images */
    HBufC* TitleForImageFetcherL() const;

    /** @return view title when fetcher is opened for selecting videos */
    HBufC* TitleForVideoFetcherL() const;
    
    /** 
     * @param aSingleSelectionTitleResourceId Resource id for view title when 
     *                                        single selection is active
     * @param aMultiSelectionTitleResourceId Resource id for view title when 
     *                                       multiselection is active
     * @return view title depending whether fetcher is in multiselection or 
     *         single selection mode, and whether client has defined the title
     */
    HBufC* TitleL( TInt aSingleSelectionTitleResourceId, 
        TInt aMultiSelectionTitleResourceId ) const;   
        
    // From MMgFetchCanceler
 	void CancelFetcherL();
 
private:
     /// Resource offset
	TInt   iResourceOffset;
        
     /// media type   
     TMediaFileType iMediaType;
	
    /// Is multiselection active or not
    /// ETrue multiselection
    /// EFalse single selection
    TBool iIsMultiSelection;

    /**
     * Left softkey label
     * Own.
     */
    HBufC* iSelectionSoftkey;

    /**
     * Popup heading text
     * Own.
     */
    HBufC* iTitle;
   
    /**
     * Verifier
     */
    MMGFetchVerifier* iVerifier;
    
    TGlxFilterItemType iFilterType;
	
	/**
     * Fetcher dialog
     */
	CGlxFetcherDialog* iDialog;
    };

#endif //GLXFETCHER_H_