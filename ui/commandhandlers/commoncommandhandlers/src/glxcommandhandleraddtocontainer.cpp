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
#include <QDebug>
#include <glxfilterfactory.h>


#include <hblistwidget.h>
#include <hbview.h>
#include <hbpushbutton.h>
#include <QGraphicsGridLayout>
#include <hbdialog.h>

TInt GlxCommandHandlerAddToContainer::iSelectionCount = 0;

const TInt KSelectionPopupListHierarchy = 5;

GlxQueryContentWidget::GlxQueryContentWidget(QGraphicsItem* parent):QGraphicsWidget(parent),
						mButton(0), mListView(0), mGrid(0)
{
    mGrid = new QGraphicsGridLayout;
    mGrid->setContentsMargins(0,0,0,0);
    
    mButton = new HbPushButton("New Item");      
    mListView = new HbListView(this);
    mListView->setSelectionMode(HbAbstractItemView::MultiSelection);
    mListView->setMinimumSize(QSize(360, 180));
    mListView->setMaximumSize(QSize(360, 180));

    mGrid->addItem(mButton, 0,0);
    mGrid->addItem(mListView, 1,0);
    setLayout(mGrid); //ownership transfered
    
    setPreferredWidth(360);
}

GlxQueryContentWidget::~GlxQueryContentWidget()
{
    qDebug("GlxQueryContentWidget::~GlxQueryContentWidget() ");
    delete mListView;
    delete mButton;
    qDebug("GlxQueryContentWidget::~GlxQueryContentWidget() exit");
}

GlxCommandHandlerAddToContainer::GlxCommandHandlerAddToContainer():mNewMediaAdded(false)
{
    mTargetContainers = NULL;
    qDebug("GlxCommandHandlerAddToContainer::GlxCommandHandlerAddToContainer() ");
}

GlxCommandHandlerAddToContainer::~GlxCommandHandlerAddToContainer()
{
    delete mTargetContainers;
    qDebug("GlxCommandHandlerAddToContainer::~GlxCommandHandlerAddToContainer() ");
}

CMPXCommand* GlxCommandHandlerAddToContainer::CreateCommandL(TInt aCommandId, MGlxMediaList& aMediaList, TBool& aConsume) const 
{
    qDebug("GlxCommandHandlerAddToContainer::CreateCommandL");
    iSelectionCount = 0;

    CMPXCollectionPath* sourceItems = aMediaList.PathLC( NGlxListDefs::EPathFocusOrSelection );
    bool ok = false;


    //create target medialist
   	CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    path->AppendL(KGlxCollectionPluginAlbumsImplementationUid);

	CMPXFilter* filter = TGlxFilterFactory::CreateCameraAlbumExclusionFilterL();
    CleanupStack::PushL(filter);
	
    // Create the media list
    MGlxMediaList* targetMediaList =  MGlxMediaList::InstanceL( *path, 
	                  TGlxHierarchyId(KSelectionPopupListHierarchy), filter );  //todo take actual hierarchy 
    CleanupStack::PopAndDestroy( filter );
    CleanupStack::PopAndDestroy(path);
    
    //create target model
    GlxModelParm modelParm (KGlxCollectionPluginAlbumsImplementationUid, KSelectionPopupListHierarchy,EGlxFilterExcludeCamera);
    GlxAlbumModel *albumMediaModel = new GlxAlbumModel (modelParm);
    albumMediaModel->setData(QModelIndex(), (int)GlxContextSelectionList, GlxContextRole );
    
    //QModelIndexList selectedModelIndexes;
    QModelIndexList modelList = GetSelectionList(albumMediaModel,&ok);
    
    CMPXCommand* command = NULL;
    qDebug("GlxCommandHandlerAddToContainer::CreateCommandL OK %d new media added %d", ok, mNewMediaAdded);
    
    if (ok|| mNewMediaAdded) {        
        int count = modelList.count();
        
        for(int i = 0 ; i < count ; i++) {
            albumMediaModel->setData( modelList[i], modelList[i].row(), GlxSelectedIndexRole );	
        }
        
        if(!mNewMediaAdded) {
            CMPXCollectionPath* targetContainers  = targetMediaList->PathLC( NGlxListDefs::EPathFocusOrSelection );
            CleanupStack::Pop(targetContainers);
            delete mTargetContainers;
            mTargetContainers = NULL;
            mTargetContainers = targetContainers;
        }
        
        command = TGlxCommandFactory::AddToContainerCommandLC(*sourceItems, *mTargetContainers);
        CleanupStack::Pop(command); 
        mNewMediaAdded = false;
    }

    MGlxMediaList::UnmarkAllL(*targetMediaList);
    targetMediaList->Close();
    CleanupStack::PopAndDestroy(sourceItems);
    delete albumMediaModel;
    return command;
}

QModelIndexList GlxCommandHandlerAddToContainer::GetSelectionList(GlxAlbumModel *model,bool *ok) const
{
    // Create a popup
    HbDialog popup;

    // Set dismiss policy that determines what tap events will cause the popup
    // to be dismissed
    popup.setDismissPolicy(HbDialog::NoDismiss);

    // Set timeout to zero to wait user to either click Ok or Cancel
    popup.setTimeout(HbDialog::NoTimeout);
    popup.setHeadingWidget(new HbLabel("Selection List"));

    GlxQueryContentWidget* view = new GlxQueryContentWidget();
    view->mListView->setModel(model);
    
    qDebug("GlxCommandHandlerAddToContainer::GetSelectionList model setting done row count %d",model->rowCount() );
    connect(view->mButton, SIGNAL(released ()), &popup, SLOT(close()));
    connect(view->mButton, SIGNAL(released ()), this, SLOT(createNewMedia()));

    HbAction *primary = new HbAction("OK"); 
    popup.setPrimaryAction(primary);

    HbAction *secondary = new HbAction("Cancel"); 
    popup.setSecondaryAction(secondary);
    
    popup.setContentWidget(view); //ownership transfer
    qDebug("GlxCommandHandlerAddToContainer::GetSelectionList after popup.setContentWidget ");
    view->mListView->show();
    QModelIndexList selectedIndexes;
    
    do {
        HbAction* action = popup.exec();
        if(action == popup.primaryAction()){
            *ok = true;
        }
        else {
            *ok = false;
        }
        
        qDebug("GlxCommandHandlerAddToContainer::GetSelectionList after exec user action %d ", *ok);
    
        if( *ok ) {
            GlxQueryContentWidget* cWidget = qobject_cast<GlxQueryContentWidget*>(popup.contentWidget());
            QItemSelectionModel* selModel = cWidget->mListView->selectionModel();
            qDebug("GlxCommandHandlerAddToContainer::GetSelectionList selection model %d", selModel);
            selectedIndexes = selModel->selectedIndexes();
        }
    }  while ( *ok && selectedIndexes.count() == 0); //continue until user select one album list or new item

    delete primary;
    delete secondary;
    qDebug("GlxCommandHandlerAddToContainer::GetSelectionList exit ");  
    
    return selectedIndexes;
}

void GlxCommandHandlerAddToContainer::createNewMedia()
{
    qDebug("GlxCommandHandlerAddToContainer::createNewMedia ");
    GlxCommandHandlerNewMedia* commandHandlerNewMedia = new GlxCommandHandlerNewMedia();
    TGlxMediaId newMediaId;
    TInt error  = commandHandlerNewMedia->ExecuteLD(newMediaId);
    
    if (error == KErrNone) {
        CMPXCollectionPath* path = CMPXCollectionPath::NewL();
        CleanupStack::PushL( path );
        path->AppendL(KGlxCollectionPluginAlbumsImplementationUid);
        path->AppendL(newMediaId.Value());
        CleanupStack::Pop(path);
        delete mTargetContainers;
        mTargetContainers = NULL;
        mTargetContainers = path;
        mNewMediaAdded = true;
    }
}
