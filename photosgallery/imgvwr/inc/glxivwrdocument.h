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
 * Description:    Document class 
 *
 */




#ifndef C_GLXIVWRDOCUMENT_H
#define C_GLXIVWRDOCUMENT_H

#include <AknDoc.h>

// Forward references
class CEikApplication;
class MMPXViewUtility;
class CGlxImageViewerManager;

/**
 *  CViewerDocument
 *
 *  @lib ViewerApplication
 */
class CGlxIVwrDocument : public CAknDocument
    {
public:
    static CGlxIVwrDocument* NewL(CEikApplication& aApp);
    static CGlxIVwrDocument* NewLC(CEikApplication& aApp);
    ~CGlxIVwrDocument();

public: // from CAknDocument
    CEikAppUi* CreateAppUiL();

public: // from CEikDocument
    CFileStore* OpenFileL( TBool aDoOpen, const TDesC& aFilename, RFs& aFs );
    void OpenFileL( CFileStore*& aFileStore, RFile& aFile );

private:
    void ConstructL();

    CGlxIVwrDocument(CEikApplication& aApp);
    void ResetDocument();

private:
    MMPXViewUtility* iViewUtility;
    CGlxImageViewerManager* iImageViewerInstance;
    };

#endif // C_GLXIVWRDOCUMENT_H
