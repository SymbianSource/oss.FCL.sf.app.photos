/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#include <mpxcollectionpath.h>
#include <mglxmedialist.h>
#include <glxcommandfactory.h>
#include <glxcommandhandlernewmedia.h>
#include <glxcommandhandlers.hrh>

#include <hbinputdialog.h>

GlxCommandHandlerNewMedia::GlxCommandHandlerNewMedia():iNewMediaCreationError(KErrNone) , iNewMediaItemTitle(0)
{
    qDebug("GlxCommandHandlerNewMedia::GlxCommandHandlerNewMedia() ");
    iSchedulerWait = new (ELeave) CActiveSchedulerWait();
}

GlxCommandHandlerNewMedia::~GlxCommandHandlerNewMedia()
{
    qDebug("GlxCommandHandlerNewMedia::~GlxCommandHandlerNewMedia() ");
    delete iSchedulerWait;
    delete iNewMediaItemTitle;
}

CMPXCommand* GlxCommandHandlerNewMedia::CreateCommandL(TInt aCommandId, MGlxMediaList& aMediaList, TBool& aConsume) const 
{
    qDebug("GlxCommandHandlerNewMedia::CreateCommandL");
    CMPXCollectionPath* path = aMediaList.PathLC( NGlxListDefs::EPathParent );
    CMPXCommand* command = NULL;
    
    _LIT(KName, "TEST");
    HBufC* newMediaItemTitle = HBufC::NewL(40);
    TPtr newMediaItemTitleDes = newMediaItemTitle->Des();
    newMediaItemTitleDes.Append(KName);
    
    delete iNewMediaItemTitle;
    iNewMediaItemTitle= NULL;
    iNewMediaItemTitle = newMediaItemTitle;

    QString title("NEW MEDIA");
    QString mainPane = QString("Album %1").arg(qrand());
    bool ok = false;

    QString mediaTitle = HbInputDialog::getText(title,  mainPane,  &ok);
    qDebug("GlxCommandHandlerNewMedia::CreateCommandL %d", ok);
    
    if(ok == true) {
        TPtr newMediaItemTitleDes = iNewMediaItemTitle->Des();
        newMediaItemTitleDes = (reinterpret_cast<const TUint16*>(mediaTitle.utf16()));
        command = TGlxCommandFactory::AddContainerCommandLC(*iNewMediaItemTitle, path->Id(0));
        CleanupStack::Pop(command);
    }
    else {
    	iNewMediaCreationError = KErrCancel;
    }
    	
    CleanupStack::PopAndDestroy(path);
    return command;
}


TInt GlxCommandHandlerNewMedia::ExecuteLD(TGlxMediaId& aNewMediaId)
{
    qDebug("GlxCommandHandlerNewMedia::ExecuteLD() ");
    GlxMpxCommandHandler::executeCommand( EGlxCmdAddMedia,KGlxAlbumsMediaId);

    if (iNewMediaCreationError == KErrNone) {
        // The user pressed OK on the dialog. We need to wait for DoHandleCommandComplete()	
       iSchedulerWait->Start();
		
       if (iNewMediaCreationError == KErrNone) {
            aNewMediaId = iNewMediaId;
        }
    }
	
    TInt error = iNewMediaCreationError;
    return error;
}

// -----------------------------------------------------------------------------
// CGlxCommandHandlerNewMedia::DoHandleCommandCompleteL
// -----------------------------------------------------------------------------
//	
void GlxCommandHandlerNewMedia::DoHandleCommandCompleteL(TAny* /*aSessionId*/, CMPXCommand* aCommandResult, 
            							TInt aError, MGlxMediaList* /*aList*/)
{
    qDebug("GlxCommandHandlerNewMedia::DoHandleCommandCompleteL() ");
    if (aError == KErrNone && aCommandResult && aCommandResult->IsSupported(KMPXMediaGeneralId)) {	
        iNewMediaId = TGlxMediaId(aCommandResult->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
    }	

    if (iSchedulerWait && aError != KErrAlreadyExists ) {
        // if iSchedulerWait exists then we know the command is being executed from
        // the ExecuteLD() method.
        iNewMediaCreationError = aError;
        iSchedulerWait->AsyncStop();
    }
}

// -----------------------------------------------------------------------------
// HandleItemAddedL
// -----------------------------------------------------------------------------
//  
void GlxCommandHandlerNewMedia::HandleItemAddedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList)
{
    qDebug("GlxCommandHandlerNewMedia::HandleItemAddedL() ");
    if(/*aList == &MediaList() && */iNewMediaId != KGlxCollectionRootId) {
         for (TInt i = aStartIndex; i <= aEndIndex; i++) {
            if (aList->Item(i).Id() == iNewMediaId) {
                qDebug("GlxCommandHandlerNewMedia::HandleItemAddedL()::SetFocusL index= %d ",i);
                aList->SetFocusL(NGlxListDefs::EAbsolute, i); // calls CGlxCommandHandlerNewMedia::SetFocusL asynchronously
                if (iSchedulerWait /*&& aError != KErrAlreadyExists*/ ) {
                    // if iSchedulerWait exists then we know the command is being executed from
                    // the ExecuteLD() method.
                    iNewMediaCreationError = KErrNone;
                    iSchedulerWait->AsyncStop();
                }

                break;
            }
        }
    }
}
