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
#include "glxprogressindicator.h"
#include <glxtracer.h>
#include <glxlog.h>
#include <StringLoader.h>
#include <AknProgressDialog.h>
#include <eikprogi.h>
#include <e32property.h>
#include <glxuiutilities.rsg>
#include "glxuiutility.h"
const TInt KPeriodicStartDelay = 500000;


// ---------------------------------------------------------
// CGlxProgressIndicator::NewL
// ---------------------------------------------------------
//  
EXPORT_C CGlxProgressIndicator* CGlxProgressIndicator::NewL
                    (MDialogDismisedObserver& aGlxGridViewNotifyObserver)
    {
    TRACER("CGlxProgressIndicator::NewL()");
    CGlxProgressIndicator* self = CGlxProgressIndicator::NewLC
                                                (aGlxGridViewNotifyObserver);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CGlxProgressIndicator::NewLC
// ---------------------------------------------------------
//  
CGlxProgressIndicator* CGlxProgressIndicator::NewLC
                    (MDialogDismisedObserver& aGlxGridViewNotifyObserver)
    {
    TRACER("CGlxProgressIndicator::NewLC()");
    CGlxProgressIndicator* self = new(ELeave)
                    CGlxProgressIndicator(aGlxGridViewNotifyObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CGlxProgressIndicator::CGlxProgressIndicator
// ---------------------------------------------------------
// 
CGlxProgressIndicator::CGlxProgressIndicator
          (MDialogDismisedObserver& aGlxGridViewNotifyObserver)
                    :iGlxGridViewNotifyObserver(aGlxGridViewNotifyObserver)
    {
    }

// ---------------------------------------------------------
// CGlxProgressIndicator::~CGlxProgressIndicator
// ---------------------------------------------------------
//
CGlxProgressIndicator::~CGlxProgressIndicator()
    {
    TRACER("CGlxProgressIndicator::~CGlxProgressIndicator()");
    if (iProgressbarTicker)
        {
        iProgressbarTicker->Cancel();
        delete iProgressbarTicker;
        }
    if(iProgressDialog)
        {
        iProgressDialog->ProcessFinishedL();
        }

	// Stop force generation of thumbnails when progress dialog is dismissed
    if (iUiUtility)
        {
        iUiUtility->StopTNMDaemon();
        iUiUtility->Close();
        }
    }

// ---------------------------------------------------------
// CGlxProgressIndicator::ConstructL
// ---------------------------------------------------------
//
void CGlxProgressIndicator::ConstructL()
    {
    TRACER("CGlxProgressIndicator::ConstructL()");
    iUiUtility->StartTNMDaemon();

    if(!iProgressbarTicker)
        {
        iProgressbarTicker = CPeriodic::NewL(CActive::EPriorityStandard);
        }

    iFinalCount = iUiUtility->GetItemsLeftCount();
    GLX_LOG_INFO1("final count in viewactivate = %d",iFinalCount);

    }


// -----------------------------------------------------------------------------
// Callback from periodic timer
// -----------------------------------------------------------------------------
//
TInt CGlxProgressIndicator::PeriodicCallbackL(TAny* aPtr )
    {
    TRACER("CGlxProgressIndicator::PeriodicCallbackL");
    static_cast< CGlxProgressIndicator* >( aPtr )->DisplayProgressBarL();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Callback from periodic timer-- non static
// -----------------------------------------------------------------------------
//
inline void CGlxProgressIndicator::DisplayProgressBarL()
    {
    TRACER("CGlxProgressIndicator::DisplayProgressBarL");
    TInt itemsLeft = iUiUtility->GetItemsLeftCount();
    GLX_LOG_INFO1("itemsLeft in DisplayProgressBarL = %d",iFinalCount);
    UpdateProgressBar();
    if(!itemsLeft)
        {
        if(iProgressbarTicker->IsActive())
            {
            iProgressbarTicker->Cancel();
            }
        StartProgressNoteL(iFinalCount,EFalse);
        }
    }

// -----------------------------------------------------------------------------
// StartProgressNoteL
// -----------------------------------------------------------------------------
//
void CGlxProgressIndicator::StartProgressNoteL(TInt aFinalValue,TBool aShow)
    {
    TRACER("CGlxProgressIndicator::StartProgressNoteL()");
    TInt itemsLeft = iUiUtility->GetItemsLeftCount();
    if(aShow)
        {
        if(!iProgressDialog)
            {
            iProgressDialog = new (ELeave) CAknProgressDialog(
                    (reinterpret_cast<CEikDialog**> (&iProgressDialog)),
                    ETrue);
            }
        iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
        
        iProgressInfo = iProgressDialog->GetProgressInfoL();
        
        iProgressDialog->SetCallback(this);
        
        HBufC* processingInfo = 
                    StringLoader::LoadLC(R_GLX_MAIN_LIST_VIEW_PROCESSING_DIALOG);
        iProgressDialog->SetTextL(*processingInfo);
        CleanupStack::PopAndDestroy(processingInfo );
        iProgressInfo->SetFinalValue(aFinalValue);
        iProgressInfo->SetAndDraw(iFinalCount-itemsLeft);
        iProgressDialog->RunLD();
        }
    else
        {
        iProgressDialog->ProcessFinishedL();
        iGlxGridViewNotifyObserver.HandleDialogDismissedL();
        if(iProgressDialog)
            {
            iProgressDialog = NULL;
            iProgressInfo = NULL;
            }
        }
     
    }
    
// -----------------------------------------------------------------------------
// UpdateProgressBar
// -----------------------------------------------------------------------------
//
void CGlxProgressIndicator::UpdateProgressBar()
    {   
    TRACER("CGlxProgressIndicator::UpdateProgressBar()");
    if(iProgressInfo)
        {
        /*
         * to show a number string in the progress bar use the below code
         * in rss it should have %d or %N format
         * TBuf<256> text;
         * GlxGeneralUiUtilities::FormatString(text,*processingInfo,-1,count,ETrue);
         * CleanupStack::PopAndDestroy(processingInfo); 
         */
        iProgressInfo->SetAndDraw(CalculateDisplayBarIncrement());
        }
    }
    
// -----------------------------------------------------------------------------
// DialogDismissedL
// -----------------------------------------------------------------------------
//  
void CGlxProgressIndicator::DialogDismissedL(TInt aButtonId)
    {
    TRACER("CGlxProgressIndicator::DialogDismissedL()");
    if(iProgressbarTicker)
        {
        iProgressbarTicker->Cancel();
        iProgressbarTicker = NULL;
        }

    if(iProgressDialog)
        {
        iProgressDialog = NULL;
        iProgressInfo = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CalculateDisplayBarIncrement
// -----------------------------------------------------------------------------
//  
TInt CGlxProgressIndicator::CalculateDisplayBarIncrement()
    {
    TRACER("CGlxProgressIndicator::CalculateDisplayBarIncrement()");

    TInt itemsLeft = iUiUtility->GetItemsLeftCount();

    if (iFinalCount < itemsLeft)
        {
        iProgressInfo->SetFinalValue(itemsLeft);
        iFinalCount = itemsLeft;
        }
    
    GLX_LOG_INFO1("CalculateDisplayBarIncrement = %d ",(iFinalCount - itemsLeft));
    return (iFinalCount - itemsLeft);
    }

// -----------------------------------------------------------------------------
// ShowProgressbar
// -----------------------------------------------------------------------------
//
void EXPORT_C CGlxProgressIndicator::ShowProgressbarL()
    {
    TRACER("CGlxProgressIndicator::ShowProgressbarL");
    iUiUtility->StartTNMDaemon();
    TInt itemsLeft = iUiUtility->GetItemsLeftCount();

    GLX_LOG_INFO1("ShowProgressbar itemsLeft = %d ",itemsLeft);
    GLX_LOG_INFO1("ShowProgressbar iFinalCount = %d ",iFinalCount);
    if (iFinalCount < itemsLeft)
        {
        /*
         *if user is in List view and inserts the MMC,update the total count 
         */
        iFinalCount = itemsLeft;
        }
    if (iFinalCount)
        {
        StartProgressNoteL(iFinalCount, ETrue);
        if (!iProgressbarTicker)
            {
            iProgressbarTicker = CPeriodic::NewL(CActive::EPriorityStandard);
            }
        if (!iProgressbarTicker->IsActive())
            {
            iProgressbarTicker->Start(KPeriodicStartDelay,
                    KPeriodicStartDelay, TCallBack(&PeriodicCallbackL,
                            static_cast<TAny*> (this)));
            }
        }
    else
        {
        GLX_LOG_INFO("Reset the RProperty flag to EFalse");
        iUiUtility->StopTNMDaemon();
        }
    }
