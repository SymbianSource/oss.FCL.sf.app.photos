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
* Description:  glximgvwrmetadatadialog.h
*
*/



#ifndef GLXIMGVWRMETADATADIALOG_H_
#define GLXIMGVWRMETADATADIALOG_H_



// SYSTEM INCLUDES
#include <AknDialog.h>

#include <alf/alfscreenbuffer.h>
#include <glximgvwrmetadatacontainer.h>
// FORWARD DECLARATIONS
class CGlxMetadataCommandHandler;
class CGlxCommandHandlerAddToContainer;
class CGlxUiUtility;

// CLASS DECLARATION
/**
 *  CGlxImgVwrMetadataDialog
 * 
 *  Metadata dialog implementation
 */
class CGlxImgVwrMetadataDialog : public CAknDialog

    {
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aURI uri or file name of item
     */
    IMPORT_C static CGlxImgVwrMetadataDialog* NewL( const TDesC& aUri );

    /**
     * Destructor.
     */
    virtual ~CGlxImgVwrMetadataDialog();

private :
    CGlxImgVwrMetadataDialog(const TDesC& aUri);


public: // Functions from base classes

    /**
     * Initializes the dialog and calls CAknDialog's 
     *   ExecuteLD()
     * This method need to be called to execute the dialog.
     * After exiting the dialog gets automatically destroyed
     *
     * @return TInt  ID value of the button that closed the dialog.
     */
    IMPORT_C TInt ExecuteLD();

    
protected:  // Functions from base classes

    /**
     * From MEikCommandObserver Prosesses menucommands
     *
     * @param aCommandId     Commant value defined in resources.
     */
    void ProcessCommandL( TInt aCommandId );

    /**
     * From MEikMenuObserver Initializes items on the menu
     *
     * @param aMenuId    Current menu's resource Id
     * @param aMenuPane  Pointer to menupane
     */
    void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);
    //hanldes orientation changes.
    void HandleResourceChange( TInt aType );
    void SizeChanged();

private:        
    /**
     * Initializes the dialog's controls before the dialog is sized and 
     * layed out. Empty by default.
     */
    void PreLayoutDynInitL();

    /**
     * Initializes the dialog's controls after the dialog has been sized 
     * but before it has been activated.
     */
    void PostLayoutDynInitL();
    /**
     * Symbian 2nd phase constructor.
     * 
     * @param aURI
     */
    void ConstructL();

    /**
     * Constructs the Alf Environment and display
     */
    void ConstructAlfEnvL();

    /**
     * Initalise the resource
     *
     */
    void InitResourceL();   

    /** 
     * To deal with any specific commands
     *
     * @param aCommand The command to respond to
     * @return ETrue if the command has been handled.
     *
     */
    TBool HandleViewCommandL(TInt aCommand);



private:

    void Draw( const TRect& /*aRect*/ ) const;    
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);       
    /**
     * SetTitleL
     * Sets title for view
     * @param aTitleText title of view to be set
     */
    void SetTitleL(const TDesC& aTitleText);    
    /**
     * SetPreviousTitleL
     * Sets title of previous view
     */
    void SetPreviousTitleL();

    void SetDetailsDlgToolbarVisibility(TBool aVisible);

public: // from MEikDialogPageObserver
    /**
     * @ref MEikDialogPageObserver::CreateCustomControlL
     */
    SEikControlInfo CreateCustomControlL(TInt aControlType);

public:
    /*
     *  
     */
    void HandleToolbarResetting(TBool aVisible);

private: //data

    TInt iResourceOffset;
    TBool iStatusPaneAvailable;
    CGlxUiUtility* iUiUtility;      
    HBufC* iPreviousTitle;    
    const TDesC& iUri; 
    CGlxImgVwrMetadataContainer* iContainer;
    };


#endif /* GLXIMGVWRMETADATADIALOG_H_ */
