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
* Description:    Progressdialog
*
*/

#ifndef GLXPROGRESSINDICATOR_H_
#define GLXPROGRESSINDICATOR_H_

#include <e32base.h>
#include <AknWaitDialog.h>

class CAknProgressDialog;
class CEikProgressInfo;
class CGlxUiUtility;

class MDialogDismisedObserver
    {
public:
    virtual void HandleDialogDismissedL()=0;
    };

class CGlxProgressIndicator : public CBase,
                                 public MProgressDialogCallback
    {
public:
    IMPORT_C static CGlxProgressIndicator* NewL(MDialogDismisedObserver&  aGlxGridViewNotifyObserver);
    ~CGlxProgressIndicator();
    static CGlxProgressIndicator* NewLC(MDialogDismisedObserver&  aGlxGridViewNotifyObserver);

private:
    CGlxProgressIndicator(MDialogDismisedObserver&  aGlxGridViewNotifyObserver);
    void ConstructL();
    
    /**
     * show the progress bar
     * @param aFinalValue Sets the final value of the bar
     * @param aShow hide/display the progress bar 
     */
    void StartProgressNoteL(TInt aFinalValue ,TBool aShow);
    /**
     * displays the increment of progress bar
     */
    void UpdateProgressBar();

    /**
     * calculates how much increment to be done in progress bar
     * @return increment value
     */
    TInt CalculateDisplayBarIncrement();
public:
    // Callback for periodic timer, static, 
    static TInt PeriodicCallbackL( TAny* aPtr );
    
    //nonstatic func called from periodic timer
    void DisplayProgressBarL();
    
    //show progress bar
    IMPORT_C void ShowProgressbarL();
    
protected://MProgressDialogCallback
    void DialogDismissedL (TInt aButtonId);
    
private:
    //Progress bar updation ticker
    CPeriodic* iProgressbarTicker;

    //Progress bar
    CAknProgressDialog* iProgressDialog;
    
    //Progress info
    CEikProgressInfo* iProgressInfo;
    
    //final count to set in progress bar
    TInt iFinalCount;
    MDialogDismisedObserver&  iGlxGridViewNotifyObserver;
    CGlxUiUtility* iUiUtility;

    };
#endif /* GLXPROGRESSINDICATOR_H_ */
