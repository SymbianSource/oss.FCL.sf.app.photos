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
* Description:    Abstraction for details data provider
*
*/




#ifndef _GLXDETAILSMULMODELPROVIDER_H_
#define _GLXDETAILSMULMODELPROVIDER_H_

#include<e32base.h>                     // Container Base Class
#include "mglxmedialist.h"              // lib glxmedialists.lib
#include <mul/imulwidget.h>

// Forward declarations
class CAlfEnv;                          // This is the UI Accelerator Toolkit environment object
class CEikMenuPane;                     // Menu panes are opened by activating the menu title
class CGlxDetailsMulModelProviderImpl;  // mplementation class of details data provider
class TGlxMulBindingSetFactory;         // Binding set Factory object
/** Error Id EMPY-7MKDHP **/
class MGlxMetadataDialogObserver ;		// call back to  details dailog

namespace Alf
	{
	class IMulWidget;                   // An interface for all Multimedia widgets
	} 

/**
 * CGlxDetailsMulModelProvider 
 * This class provides the necessary abstraction of the details provider 
 * the implementation is provided by CGlxDetailsMulModelProviderImpl class
 */
NONSHARABLE_CLASS( CGlxDetailsMulModelProvider ) : public CBase
	{
public:

	/**
	 * Constructor
	 * @param aEnv Env variable of ALF
	 * @param aWidget Alf Widget type
	 * @param aMediaList medialist from which attributes to be fetched
	 * @param aDefaultTemplate Template that will be set to model
	 */
	IMPORT_C static CGlxDetailsMulModelProvider* NewL( CAlfEnv& aEnv,
											Alf::IMulWidget& aWidget, 
											MGlxMediaList& aMediaList, 
											Alf::mulwidget::TLogicalTemplate aDefaultTemplate,
											TInt aDataWindowSize );
											
    /** ConstructL
 	 * @param aEnv Env variable of ALF
 	 * @param aWidget Alf Widget type
 	 * @param aMediaList medialist from which attributes to be fetched
 	 * @param aDefaultTemplate Template that will be set to model
 	 */
    void ConstructL(
        CAlfEnv& aEnv,	
        Alf::IMulWidget& aWidget, 
        MGlxMediaList& aMediaList, 
        Alf::mulwidget::TLogicalTemplate aDefaultTemplate, 
        TInt aDataWindowSize);
        
	/**
	 * Destructor
	 */
	IMPORT_C ~CGlxDetailsMulModelProvider();	
  
	/**
	 * This Api is called from client with the required factory instance
	 * This will be called as many number of times as the number of items present in details view
	 * @param aFactory factory instance
	 */
	IMPORT_C void AddEntryL( const TGlxMulBindingSetFactory& aFactory );
	
	/**
	 * OfferCommandL
	 * This will be called from view whenever there is an event (for example: navi key pressed )
	 * @param aCommandId Id of the command that is about to be executed
	 */
	IMPORT_C TBool OfferCommandL( TInt aCommandId );
	
	/**
	 * InitMenuL
	 * This will determine what options to be present in menu pane
	 * @param aMenu The menu pane to edit
	 */
	IMPORT_C void InitMenuL( CEikMenuPane& aMenu );
	
    /**
     * HideLocationMenuItem
     * @param aMenu The menu pane to edit
     */
	IMPORT_C void HideLocationMenuItem( CEikMenuPane& aMenu );
	
	/**
	 * EnableMskL
	 * This will determine which Middle softkey has to be displayed
	 */
	IMPORT_C void EnableMskL();

    /**
     * Set Details Dailog Observer
     * @param aMenu The menu pane to edit
     */
     /** Error Id EMPY-7MKDHP **/
	IMPORT_C void SetDetailsDailogObserver( MGlxMetadataDialogObserver& aObserver ) ;

private:
	
	// Implemented class instance: owns
	CGlxDetailsMulModelProviderImpl* iImpl; 
	};
	
#endif // _GLXDETAILSMULMODELPROVIDER_H_
