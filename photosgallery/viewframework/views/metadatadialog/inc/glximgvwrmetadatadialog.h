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

#include <glximgvwrmetadatacontainer.h>


// CLASS DECLARATION
/**
 *  CGlxImgVwrMetadataDialog
 * 
 *  Image Viewer Metadata dialog implementation
 */
class CGlxImgVwrMetadataDialog : public CAknDialog
    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aURI uri or file name of item
     */
    IMPORT_C static CGlxImgVwrMetadataDialog* NewL(const TDesC& aUri);

    /**
     * Destructor.
     */
    virtual ~CGlxImgVwrMetadataDialog();

private:
    /**
     * Constructor.
     * @param aURI uri or file name of item
     */  
    CGlxImgVwrMetadataDialog(const TDesC& aUri);

public:
    // Functions from base classes

    /**
     * Initializes the dialog and calls CAknDialog's 
     *   ExecuteLD()
     * This method need to be called to execute the dialog.
     * After exiting the dialog gets automatically destroyed
     *
     * @return TInt  ID value of the button that closed the dialog.
     */
    IMPORT_C TInt ExecuteLD();

protected:
    /**
     * From MEikCommandObserver Prosesses menucommands
     *
     * @param aCommandId     Commant value defined in resources.
     */
    void ProcessCommandL(TInt aCommandId);

private:
    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Initalise the resource
     */
    void InitResourceL();

private:
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

    /** 
    * Sets the dialog toolbar visibility
    * @param aVisible - ETrue for visible; EFalse otherwise.
    */
    void SetDetailsDlgToolbarVisibility(TBool aVisible);

public:
    // from MEikDialogPageObserver
    /**
     * @ref MEikDialogPageObserver::CreateCustomControlL
     */
    SEikControlInfo CreateCustomControlL(TInt aControlType);

public:
    /** 
    * Handles the toolbar visiblity to be in sync with the calling app
    * @param aVisible - ETrue for visible; EFalse otherwise.
    */    
    void HandleToolbarResetting(TBool aVisible);

private:
    //data
    TInt iResourceOffset;
    TBool iStatusPaneAvailable;
    HBufC* iPreviousTitle;
    const TDesC& iUri;
    CGlxImgVwrMetadataContainer* iContainer;
    };


#endif /* GLXIMGVWRMETADATADIALOG_H_ */
