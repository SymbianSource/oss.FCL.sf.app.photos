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
#include <glxcommandhandleraddtocontainer.h>
#include <glxcommandhandlernewmedia.h>

#include <QStringListModel>
#include <hblabel.h>
#include <hbaction.h>

#include <glxalbummodel.h>
#include <glxcollectionpluginalbums.hrh>
#include <glxmodelparm.h>
#include <glxfilterfactory.h>

#include <hblistwidget.h>
#include <hbview.h>
#include <hbpushbutton.h>
#include <QGraphicsGridLayout>
#include <hbdialog.h>
#include <hbmessagebox.h>

#include "glxlocalisationstrings.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxcommandhandleraddtocontainerTraces.h"
#endif


TInt GlxCommandHandlerAddToContainer::iSelectionCount = 0;

const TInt KSelectionPopupListHierarchy = 5;
const TInt KListPrefferedHeight = 400;

GlxQueryContentWidget::GlxQueryContentWidget(QGraphicsItem* parent) :
    QGraphicsWidget(parent), mButton(0), mListView(0), mGrid(0)
    {
    OstTraceFunctionEntry0( DUP1_GLXQUERYCONTENTWIDGET_GLXQUERYCONTENTWIDGET_ENTRY );
    mGrid = new QGraphicsGridLayout;
    mGrid->setContentsMargins(0, 0, 0, 0);

    mButton = new HbPushButton("New Item");
    mListView = new HbListView(this);
    mListView->setSelectionMode(HbAbstractItemView::MultiSelection);
    mListView->setPreferredHeight(KListPrefferedHeight);

    mGrid->addItem(mButton, 0, 0);
    mGrid->addItem(mListView, 1, 0);
    setLayout(mGrid); //ownership transfered

    OstTraceFunctionExit0( DUP1_GLXQUERYCONTENTWIDGET_GLXQUERYCONTENTWIDGET_EXIT );
    }

GlxQueryContentWidget::~GlxQueryContentWidget()
    {
    OstTraceFunctionEntry0( GLXQUERYCONTENTWIDGET_GLXQUERYCONTENTWIDGET_ENTRY );
    delete mListView;
    delete mButton;
    OstTraceFunctionExit0( GLXQUERYCONTENTWIDGET_GLXQUERYCONTENTWIDGET_EXIT );
    }

GlxCommandHandlerAddToContainer::GlxCommandHandlerAddToContainer() :
    mNewMediaAdded(false)
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERADDTOCONTAINER_GLXCOMMANDHANDLERADDTOCONTAINER_ENTRY );
    mTargetContainers = NULL;
    OstTraceFunctionExit0( GLXCOMMANDHANDLERADDTOCONTAINER_GLXCOMMANDHANDLERADDTOCONTAINER_EXIT );
    }

GlxCommandHandlerAddToContainer::~GlxCommandHandlerAddToContainer()
    {
    OstTraceFunctionEntry0( DUP1_GLXCOMMANDHANDLERADDTOCONTAINER_GLXCOMMANDHANDLERADDTOCONTAINER_ENTRY );
    delete mTargetContainers;
    OstTraceFunctionExit0( DUP1_GLXCOMMANDHANDLERADDTOCONTAINER_GLXCOMMANDHANDLERADDTOCONTAINER_EXIT );
    }

CMPXCommand* GlxCommandHandlerAddToContainer::CreateCommandL(TInt /*aCommandId*/,
        MGlxMediaList& aMediaList, TBool& /*aConsume*/) const
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERADDTOCONTAINER_CREATECOMMANDL_ENTRY );
    iSelectionCount = 0;

    CMPXCollectionPath* sourceItems = aMediaList.PathLC(
            NGlxListDefs::EPathFocusOrSelection);
    bool ok = false;

    //create target medialist
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL(path);
    path->AppendL(KGlxCollectionPluginAlbumsImplementationUid);

    CMPXFilter* filter =
            TGlxFilterFactory::CreateCameraAlbumExclusionFilterL();
    CleanupStack::PushL(filter);

    // Create the media list
    MGlxMediaList* targetMediaList = MGlxMediaList::InstanceL(*path,
            TGlxHierarchyId(KSelectionPopupListHierarchy), filter); //todo take actual hierarchy 
    CleanupStack::PopAndDestroy(filter);
    CleanupStack::PopAndDestroy(path);

    //create target model
    GlxModelParm modelParm(KGlxCollectionPluginAlbumsImplementationUid,
            KSelectionPopupListHierarchy, EGlxFilterExcludeCamera);
    GlxAlbumModel *albumMediaModel = new GlxAlbumModel(modelParm);
    albumMediaModel->setData(QModelIndex(), (int) GlxContextSelectionList,
            GlxContextRole);

    QModelIndexList modelList = GetSelectionList(albumMediaModel, &ok);

    CMPXCommand* command = NULL;
    OstTraceExt2( TRACE_NORMAL, GLXCOMMANDHANDLERADDTOCONTAINER_CREATECOMMANDL, "GlxCommandHandlerAddToContainer::CreateCommandL;ok=%d;newMedia=%d", ok, mNewMediaAdded );

    if (ok || mNewMediaAdded)
        {
        int count = modelList.count();

        for (int i = 0; i < count; i++)
            {
            albumMediaModel->setData(modelList[i], modelList[i].row(),
                    GlxSelectedIndexRole);
            }

        if (!mNewMediaAdded)
            {
            CMPXCollectionPath* targetContainers = targetMediaList->PathLC(
                    NGlxListDefs::EPathFocusOrSelection);
            CleanupStack::Pop(targetContainers);
            delete mTargetContainers;
            mTargetContainers = NULL;
            mTargetContainers = targetContainers;
            }

        command = TGlxCommandFactory::AddToContainerCommandLC(*sourceItems,
                *mTargetContainers);
        CleanupStack::Pop(command);
        mNewMediaAdded = false;
        }
    else
        {
        MGlxMediaList::UnmarkAllL(aMediaList);
        }

    MGlxMediaList::UnmarkAllL(*targetMediaList);
    targetMediaList->Close();
    CleanupStack::PopAndDestroy(sourceItems);
    delete albumMediaModel;
    OstTraceFunctionExit0( GLXCOMMANDHANDLERADDTOCONTAINER_CREATECOMMANDL_EXIT );
    return command;
    }

QModelIndexList GlxCommandHandlerAddToContainer::GetSelectionList(
        GlxAlbumModel *model, bool *ok) const
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERADDTOCONTAINER_GETSELECTIONLIST_ENTRY );
    // Create a popup
    HbDialog popup;
    popup.setPreferredHeight(400);
    // Set dismiss policy that determines what tap events will cause the popup
    // to be dismissed
    popup.setDismissPolicy(HbDialog::NoDismiss);

    // Set timeout to zero to wait user to either click Ok or Cancel
    popup.setTimeout(HbDialog::NoTimeout);
    popup.setHeadingWidget(new HbLabel("Selection List"));

    GlxQueryContentWidget* view = new GlxQueryContentWidget();
    view->mListView->setModel(model);

    connect(view->mButton, SIGNAL(released ()), &popup, SLOT(close()));
    connect(view->mButton, SIGNAL(released ()), this, SLOT(createNewMedia()));


    HbAction *primary = new HbAction(GLX_BUTTON_OK);
    popup.setPrimaryAction(primary);

    HbAction *secondary = new HbAction(GLX_BUTTON_CANCEL);
    popup.setSecondaryAction(secondary);
    
    popup.setContentWidget(view); //ownership transfer
    view->mListView->show();
    
    QModelIndexList selectedIndexes;

    do
        {
        HbAction* action = popup.exec();
        if (action == popup.primaryAction())
            {
            *ok = true;
            }
        else
            {
            *ok = false;
            }
        OstTrace1( TRACE_NORMAL, GLXCOMMANDHANDLERADDTOCONTAINER_GETSELECTIONLIST, "GlxCommandHandlerAddToContainer::GetSelectionList;ok=%d", *ok );


        if (*ok)
            {
            GlxQueryContentWidget* cWidget = qobject_cast<
                    GlxQueryContentWidget*> (popup.contentWidget());
            QItemSelectionModel* selModel =
                    cWidget->mListView->selectionModel();
            selectedIndexes = selModel->selectedIndexes();
            }
        }
    while (*ok && selectedIndexes.count() == 0); //continue until user select one album list or new item

    delete primary;
    delete secondary;

    OstTraceFunctionExit0( GLXCOMMANDHANDLERADDTOCONTAINER_GETSELECTIONLIST_EXIT );
    return selectedIndexes;
    }

void GlxCommandHandlerAddToContainer::createNewMedia()
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERADDTOCONTAINER_CREATENEWMEDIA_ENTRY );
    GlxCommandHandlerNewMedia* commandHandlerNewMedia =
            new GlxCommandHandlerNewMedia();
    TGlxMediaId newMediaId;
    TInt error = commandHandlerNewMedia->ExecuteLD(newMediaId);

    while (error == KErrAlreadyExists)
        {
        HbMessageBox::warning("Name Already Exist!!!", new HbLabel(
                "New Album"));
        error = KErrNone;
        error = commandHandlerNewMedia->ExecuteLD(newMediaId);
        }

    if (error == KErrNone)
        {
        CMPXCollectionPath* path = CMPXCollectionPath::NewL();
        CleanupStack::PushL(path);
        path->AppendL(KGlxCollectionPluginAlbumsImplementationUid);
        path->AppendL(newMediaId.Value());
        CleanupStack::Pop(path);
        delete mTargetContainers;
        mTargetContainers = NULL;
        mTargetContainers = path;
        mNewMediaAdded = true;
        }
    OstTraceFunctionExit0( GLXCOMMANDHANDLERADDTOCONTAINER_CREATENEWMEDIA_EXIT );
    }

QString GlxCommandHandlerAddToContainer::CompletionTextL() const
    {
	return QString();	
    }

QString GlxCommandHandlerAddToContainer::ProgressTextL() const
    {
    return QString("Adding album...");
    }
