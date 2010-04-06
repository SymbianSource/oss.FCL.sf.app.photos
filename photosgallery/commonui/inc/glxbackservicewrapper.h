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
* Description:    wrapper for Back Stepping Service
*
*/




#ifndef C_GLX_BACKSERVICE_WRAPPER
#define C_GLX_BACKSERVICE_WRAPPER

#include <e32base.h>

class MLiwInterface;
class CLiwGenericParamList;
class CLiwServiceHandler;

/**
 *   BS Service wrapper
 *
 *  @since S60 v3.2
 */
class CGlxBackServiceWrapper : public CBase
    {

public:

    /**
     * Two-phased constructors
     * @param aUid client application UID
     */
    IMPORT_C static CGlxBackServiceWrapper* NewL( const TUid aUid );
    IMPORT_C static CGlxBackServiceWrapper* NewLC( const TUid aUid );
    
    /**
    * Destructor.
    */
    virtual ~CGlxBackServiceWrapper();

    /**
     * Forwards activation event
     *
     * @since S60 v3.2
     * @param aState state of application like view ids
     * @param aEnter indicate if it is entry/exit activation
     * @return result code retutned by BS Service interface
     * @leave leaves from HandleResultL are propagated
     */
    IMPORT_C TInt ForwardActivationEventL( const TDesC8& aState, const TBool aEnter );
    
    /**
     * Handles back command
     *
     * @since S60 v3.2
     * @param aState state of application like view ids
     * @return result code retutned by BS Service interface
     * @leave leaves from HandleResultL are propagated
     */
    IMPORT_C TInt HandleBackCommandL( const TDesC8& aState, const TBool aCheckOnly = EFalse );

private:

    CGlxBackServiceWrapper();

    /**
     * 2nd phase constructor
     * @param aUid client application UID
     */
    void ConstructL( const TUid aUid );
    
    /**
     * Initializes BS Service
     * @param aUid client application UID
     * @leave KErrNotSupported or KErrArgument 
     *        (if arguments passed to BS Service are incorrect 
     *        or could not initialize BS)
     *        KErrNotFound (if there was no return value)
     */
    void InitializeL( const TUid aUid );
    
    /**
     * Handles the result of a LIW command
     * @return result code retutned by BS Service interface
     * @leave KErrNotSupported or KErrArgument 
     *        (if arguments passed to BS Service are incorrect)
     *        KErrNotFound (if there was no return value)
     */
    TInt HandleResultL();

private: // data
    
    /**
     * Liw Service Handler
     * Own.
     */
    CLiwServiceHandler* iServiceHandler;
    
    /**
     * BS Service interface returned by LIW
     * Own.
     */
    MLiwInterface* iBSInterface;
    
    /**
     * In param list
     * Not own.
     */
    CLiwGenericParamList* iInParamList;
    
    /**
     * Out param list
     * Not own.
     */
    CLiwGenericParamList* iOutParamList;

    };

#endif // C_GLX_BACKSERVICE_WRAPPER
