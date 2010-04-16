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
#include <glxattributecontext.h>
#include <glxattributeretriever.h>
#include <glxfetchcontextremover.h>
#include <glxcollectionpluginalbums.hrh>

#include <hbinputdialog.h>
#include <hblabel.h>
#include <hbmessagebox.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxcommandhandlernewmediaTraces.h"
#endif
#include <hbaction.h>

GlxTextInputDialog::GlxTextInputDialog()
    {
    }

GlxTextInputDialog::~GlxTextInputDialog()
    {
    }

QString GlxTextInputDialog::getText(const QString &label,
        const QString &text, bool *ok)
    {
    mDialog = new HbInputDialog();
    mDialog->setPromptText(label);
    mDialog->setInputMode(HbInputDialog::TextInput);
    mDialog->setValue(text);
    connect(mDialog->lineEdit(0), SIGNAL( textChanged (const QString &) ),
            this, SLOT( textChanged (const QString &)));
    HbAction* action = mDialog->exec();
    QString retText = NULL;
    if (action == mDialog->secondaryAction())
        { //Cancel was pressed
        if (ok)
            {
            *ok = false;
            }
        }
    else
        { //OK was pressed
        if (ok)
            {
            *ok = true;
            }
        retText = mDialog->value().toString().trimmed();
        }
    disconnect(mDialog->lineEdit(0), SIGNAL( textChanged (const QString &) ),
            this, SLOT( textChanged (const QString &)));
    delete mDialog;
    mDialog = NULL;
    return retText;
    }

void GlxTextInputDialog::textChanged(const QString &text)
    {
    if (text.trimmed().isEmpty())
        {
        mDialog->primaryAction()->setEnabled(false);
        }
    else
        {
        mDialog->primaryAction()->setEnabled(true);
        }
    }


GlxCommandHandlerNewMedia::GlxCommandHandlerNewMedia() :
    iNewMediaCreationError(KErrNone), iNewMediaItemTitle(0) , mShowConfirmation(false)
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERNEWMEDIA_GLXCOMMANDHANDLERNEWMEDIA_ENTRY );
    iSchedulerWait = new (ELeave) CActiveSchedulerWait();
    OstTraceFunctionExit0( GLXCOMMANDHANDLERNEWMEDIA_GLXCOMMANDHANDLERNEWMEDIA_EXIT );
    }

GlxCommandHandlerNewMedia::~GlxCommandHandlerNewMedia()
    {
    OstTraceFunctionEntry0( DUP1_GLXCOMMANDHANDLERNEWMEDIA_GLXCOMMANDHANDLERNEWMEDIA_ENTRY );
    if (iSchedulerWait && iSchedulerWait->IsStarted())
        {
        iSchedulerWait->AsyncStop();
        }
    delete iSchedulerWait;
    delete iNewMediaItemTitle;
    OstTraceFunctionExit0( DUP1_GLXCOMMANDHANDLERNEWMEDIA_GLXCOMMANDHANDLERNEWMEDIA_EXIT );
    }

CMPXCommand* GlxCommandHandlerNewMedia::CreateCommandL(TInt /*aCommandId*/,
        MGlxMediaList& aMediaList, TBool& /*aConsume*/) const
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERNEWMEDIA_CREATECOMMANDL_ENTRY );
    CMPXCollectionPath* path = aMediaList.PathLC(NGlxListDefs::EPathParent);
    CMPXCommand* command = NULL;
   

    _LIT(KName, "TEST");
    HBufC* newMediaItemTitle = HBufC::NewL(40);
    TPtr newMediaItemTitleDes = newMediaItemTitle->Des();
    newMediaItemTitleDes.Append(KName);


    delete iNewMediaItemTitle;
    iNewMediaItemTitle = NULL;
    iNewMediaItemTitle = newMediaItemTitle;

    QString title("NEW MEDIA");
    QString mainPane = GenerateNewMediaItemTitleL("Album",aMediaList);
    bool ok = false;
	QString mediaTitle = NULL;
    GlxTextInputDialog* dlg = new GlxTextInputDialog();
    mediaTitle = dlg->getText(title, mainPane, &ok);
    delete dlg;
    iNewMediaCreationError = KErrNone;
    if (ok == true)
        {
        
        TPtrC16 newMediaItemTitleDes
                = (reinterpret_cast<const TUint16*> (mediaTitle.utf16()));
        
        delete iNewMediaItemTitle;
        iNewMediaItemTitle = NULL;        
        iNewMediaItemTitle = newMediaItemTitleDes.Alloc();
        
        command = TGlxCommandFactory::AddContainerCommandLC(
                *iNewMediaItemTitle, path->Id(0));
        CleanupStack::Pop(command);
        }
    else
        {
        iNewMediaCreationError = KErrCancel;
        }

    CleanupStack::PopAndDestroy(path);
    OstTraceFunctionExit0( GLXCOMMANDHANDLERNEWMEDIA_CREATECOMMANDL_EXIT );
    return command;
    }


TInt GlxCommandHandlerNewMedia::ExecuteLD(TGlxMediaId& aNewMediaId)
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERNEWMEDIA_EXECUTELD_ENTRY );
    GlxMpxCommandHandler::executeCommand(EGlxCmdAddMedia, KGlxCollectionPluginAlbumsImplementationUid);
    mShowConfirmation = true;
    if (iNewMediaCreationError == KErrNone)
        {
        // The user pressed OK on the dialog. We need to wait for DoHandleCommandComplete()	
        iSchedulerWait->Start();

        if (iNewMediaCreationError == KErrNone)
            {
            aNewMediaId = iNewMediaId;
            }
        }

    TInt error = iNewMediaCreationError;
    OstTraceFunctionExit0( GLXCOMMANDHANDLERNEWMEDIA_EXECUTELD_EXIT );
    return error;
    }

// -----------------------------------------------------------------------------
// CGlxCommandHandlerNewMedia::DoHandleCommandCompleteL
// -----------------------------------------------------------------------------
//	
void GlxCommandHandlerNewMedia::DoHandleCommandCompleteL(
        TAny* /*aSessionId*/, CMPXCommand* aCommandResult, TInt aError,
        MGlxMediaList* /*aList*/)
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERNEWMEDIA_DOHANDLECOMMANDCOMPLETEL_ENTRY );
    if (aError == KErrNone && aCommandResult && aCommandResult->IsSupported(
            KMPXMediaGeneralId))
        {
        iNewMediaId = TGlxMediaId(aCommandResult->ValueTObjectL<TMPXItemId> (
                KMPXMediaGeneralId));
        }
    if (iSchedulerWait && iSchedulerWait->IsStarted())
        {
        // if iSchedulerWait exists then we know the command is being executed from
        // the ExecuteLD() method.
        iNewMediaCreationError = aError;
        iSchedulerWait->AsyncStop();
        }
    
    OstTraceFunctionExit0( GLXCOMMANDHANDLERNEWMEDIA_DOHANDLECOMMANDCOMPLETEL_EXIT );
    }

// -----------------------------------------------------------------------------
// HandleItemAddedL
// -----------------------------------------------------------------------------
//  
void GlxCommandHandlerNewMedia::HandleItemAddedL(TInt aStartIndex,
        TInt aEndIndex, MGlxMediaList* aList)
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERNEWMEDIA_HANDLEITEMADDEDL_ENTRY );
    if (iNewMediaId != KGlxCollectionRootId)
        {
        for (TInt i = aStartIndex; i <= aEndIndex; i++)
            {
            if (aList->Item(i).Id() == iNewMediaId)
                {
                aList->SetFocusL(NGlxListDefs::EAbsolute, i); // calls CGlxCommandHandlerNewMedia::SetFocusL asynchronously
                if (iSchedulerWait && iSchedulerWait->IsStarted() )
                    {
                    // if iSchedulerWait exists then we know the command is being executed from
                    // the ExecuteLD() method.
                    iNewMediaCreationError = KErrNone;
                    iSchedulerWait->AsyncStop();
                    }

                break;
                }
            }
        }
    OstTraceFunctionExit0( GLXCOMMANDHANDLERNEWMEDIA_HANDLEITEMADDEDL_EXIT );
    }

void GlxCommandHandlerNewMedia::HandleError(TInt /*aError*/)
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERNEWMEDIA_HANDLEERROR_ENTRY );
    OstTraceFunctionExit0( GLXCOMMANDHANDLERNEWMEDIA_HANDLEERROR_EXIT );
    }

void GlxCommandHandlerNewMedia::HandleErrorL(TInt aErrorCode)
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERNEWMEDIA_HANDLEERRORL_ENTRY );
    if (aErrorCode == KErrAlreadyExists && !mShowConfirmation)
        {
        HbMessageBox::warning("Album Already Exist!!!" );
        }
    OstTraceFunctionExit0( GLXCOMMANDHANDLERNEWMEDIA_HANDLEERRORL_EXIT );
    }

QString GlxCommandHandlerNewMedia::GenerateNewMediaItemTitleL(
        QString newMediaTilte, MGlxMediaList& aMediaList) const
    {
    TGlxSequentialIterator iter;
    CGlxAttributeContext* attributeContext =
            new (ELeave) CGlxAttributeContext(&iter);
    CleanupStack::PushL(attributeContext);
    attributeContext->AddAttributeL(KMPXMediaGeneralTitle);
    aMediaList.AddContextL(attributeContext, KGlxFetchContextPriorityBlocking);
    TGlxFetchContextRemover contextRemover(attributeContext, aMediaList);
    // put to cleanupstack as cleanupstack is emptied before stack objects
    // are deleted
    CleanupClosePushL(contextRemover);
    User::LeaveIfError(GlxAttributeRetriever::RetrieveL(*attributeContext,
            aMediaList, ETrue));
    // context off the list
    CleanupStack::PopAndDestroy(&contextRemover);
    CleanupStack::PopAndDestroy(attributeContext);

    RArray<TInt> numbers;
    CleanupClosePushL(numbers);

    TInt count = aMediaList.Count();
    for (TInt i = 0; i < count; i++)
        {
        TGlxMedia item = aMediaList.Item(i);
        const CGlxMedia* media = item.Properties();
        if (media)
            {
            const TDesC& titleDesc = media->ValueText(KMPXMediaGeneralTitle);
            QString title = QString::fromUtf16(titleDesc.Ptr(),
                    titleDesc.Length());

            TInt length = newMediaTilte.length();
            if (title.startsWith(newMediaTilte, Qt::CaseSensitive))
                {
                if (length == title.length())
                    {
                    numbers.InsertInOrder(0); // special case
                    }
                else if (title.length() > length)
                    {
                    QString num = title.mid(length);
                    bool ok = false;
                    TInt dec = num.toInt(&ok);
                    if (ok)
                        {
                        numbers.InsertInOrder(dec);
                        }
                    }
                }
            }
        }

    TInt nextNumber = 0;
    count = numbers.Count();
    for (TInt i = 0; i < count; i++)
        {
        if (numbers[i] == nextNumber)
            {
            nextNumber++;
            }
        else
            {
            break;
            }
        }

    CleanupStack::PopAndDestroy(&numbers);

    QString newMediaItemTitle = newMediaTilte;

    if (nextNumber > 0)
        {
        newMediaItemTitle.append(QString("%1").arg(nextNumber));
        }
    return newMediaItemTitle;

}

QString GlxCommandHandlerNewMedia::CompletionTextL() const
    {
    if (!mShowConfirmation)
        {
        return QString("Album added");
        }
    return QString();
    }

QString GlxCommandHandlerNewMedia::ProgressTextL() const
    {
    if (!mShowConfirmation)
        {
        return QString("Adding album...");
        }
    return QString();
    }

