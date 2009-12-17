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
* Description:    Updation via IAD
*
*/


#ifndef GLXIADUPDATE_H_
#define GLXIADUPDATE_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <iaupdateobserver.h>

// CONSTANTS
// None

// FORWARD DECLARATIONS
class CIAUpdate;
class CIAUpdateParameters;

// CLASS DECLARATION


class CGlxIadUpdate : public CBase, public MIAUpdateObserver
    {

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CGlxIadUpdate* NewL();

    /**
     * Destructor.
     */
    virtual ~CGlxIadUpdate();
   
public: // New methods

    /**
     * Start IAD update process.
     */
    IMPORT_C void StartL();

private: // Constructors and destructor

    /**
     * C++ default constructor.
     */
    CGlxIadUpdate();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
    
private: // New methods


    /** 
     * From MIAUpdateObserver.
     * This callback function is called when the update checking operation has completed.
     *
     * @param aErrorCode The error code of the observed update operation.
     *                   KErrNone for successful completion, 
     *                   otherwise a system wide error code.
     * @param aAvailableUpdates Number of the updates that were found available.
     *
     * @since S60 v3.2
     */
    void CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates );


    /** 
     * From MIAUpdateObserver.
     * This callback function is called when an update operation has completed.
     * Even if multiple functions are provided to start different update operations,
     * this callback function is always called after an update operation has completed.
     *
     * @param aErrorCode The error code of the completed update operation.
     *                   KErrNone for successful completion, 
     *                   otherwise a system wide error code.
     * @param aResult Details about the completed update operation.
     *                Ownership is transferred.
     *
     * @since S60 v3.2
     */
    void UpdateComplete( TInt aErrorCode, CIAUpdateResult* aResultDetails );


    /** 
     * From MIAUpdateObserver.
     * This callback function is called when an update query operation has completed.
     *
     * @param aErrorCode The error code of the observed query operation.
     *                   KErrNone for successful completion, 
     *                   otherwise a system wide error code.
     * @param aUpdateNow ETrue informs that an update operation should be started.
     *                   EFalse informs that there is no need to start an update
     *                   operation.
     * @since Series 60 3.2
     */
    void UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow );


    /**
     * Clears IAD update.
     * @since Series 60 3.2
     * @param none.
     * @return none.
     */
    void ReleaseIADUpdate();

private: // Data


    /**
     * IAD update API.
     */
    CIAUpdate* iUpdate;

    /**
     * IAD update parameters.
     */
    CIAUpdateParameters* iParameters;


    };

#endif /* GLXIADUPDATE_H_ */
