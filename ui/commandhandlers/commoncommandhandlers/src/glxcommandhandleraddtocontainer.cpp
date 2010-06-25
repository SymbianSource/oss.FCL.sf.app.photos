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
#include <QEventLoop>

#include <hblistwidget.h>
#include <hbview.h>
#include <hbpushbutton.h>
#include <QGraphicsGridLayout>
#include <hbdialog.h>
#include <hbmessagebox.h>

#include <glxcommandhandlers.hrh>
#include "glxlocalisationstrings.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxcommandhandleraddtocontainerTraces.h"
#endif


TInt GlxCommandHandlerAddToContainer::iSelectionCount = 0;

const TInt KSelectionPopupListHierarchy = 5;
const TInt KListPrefferedHeight = 400;

GlxAlbumSelectionPopup::GlxAlbumSelectionPopup() 
    : mPopupDlg( 0 ), 
      mSelectionModel( 0 ),
      mEventLoop( 0 ),
      mResult( false )
{
}

GlxAlbumSelectionPopup::~GlxAlbumSelectionPopup()
{
}

QModelIndexList GlxAlbumSelectionPopup::GetSelectionList(GlxAlbumModel *model, bool *ok) 
{
    // Create a popup
    HbDialog popup;
    QEventLoop eventLoop;
    mEventLoop = &eventLoop;
    
    popup.setPreferredHeight( KListPrefferedHeight );
    // Set dismiss policy that determines what tap events will cause the popup
    // to be dismissed
    popup.setDismissPolicy(HbDialog::NoDismiss);

    // Set timeout to zero to wait user to either click Ok or Cancel
    popup.setTimeout(HbDialog::NoTimeout);
    popup.setHeadingWidget( new HbLabel("Select Album") );      
    
    mPopupDlg = &popup;
    HbListView *listview = new HbListView();
    listview->setSelectionMode(HbAbstractItemView::MultiSelection);
    listview->setModel(model);
    mSelectionModel = listview->selectionModel() ;
    connect( mSelectionModel, SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection& ) ), this, SLOT( changeButtonText() ) );
    
    HbAction *primary = new HbAction( "New" );
    primary->setObjectName( "Cmd New" );
    popup.addAction( primary ) ;

    HbAction *secondary = new HbAction( GLX_BUTTON_CANCEL );
    secondary->setObjectName( "Cmd Cancel" );
    popup.addAction( secondary );
    
    popup.setContentWidget( listview ); //ownership transfer
    listview->show();
    
    popup.open( this, SLOT( dialogClosed( HbAction* ) ) ); 
    eventLoop.exec( );
    mEventLoop = 0 ;
    if ( ok ) {
        *ok = mResult ;
    }
    QModelIndexList selectedIndexes = mSelectionModel->selectedIndexes();       
    disconnect( mSelectionModel, SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection& ) ), this, SLOT( changeButtonText() ) );
    delete primary;
    delete secondary;

    return selectedIndexes;
}

void GlxAlbumSelectionPopup::changeButtonText()
{
    if ( mSelectionModel->selectedIndexes().count() ) {
        mPopupDlg->actions().first()->setText( GLX_BUTTON_OK );
    }
    else {
        mPopupDlg->actions().first()->setText("New");
    }    
}


void GlxAlbumSelectionPopup::dialogClosed(HbAction *action)
{
    HbDialog *dlg = static_cast<HbDialog*>(sender());
    if( action == dlg->actions().first() ) {
        mResult = true ;
    }
    else {
        mResult = false ;
    }
    if ( mEventLoop && mEventLoop->isRunning( ) ) {
        mEventLoop->exit( 0 );
    }
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

CMPXCommand* GlxCommandHandlerAddToContainer::CreateCommandL(TInt aCommandId,
        MGlxMediaList& aMediaList, TBool& /*aConsume*/) const
    {
    OstTraceFunctionEntry0( GLXCOMMANDHANDLERADDTOCONTAINER_CREATECOMMANDL_ENTRY );
    iSelectionCount = 0;
    CMPXCommand* command = NULL;

    if(aCommandId == EGlxCmdAddToFav)
        {
           CMPXCollectionPath* targetCollection = CMPXCollectionPath::NewL();
           CleanupStack::PushL(targetCollection);
           // The target collection has to be appeneded with the albums plugin id
           targetCollection->AppendL(KGlxCollectionPluginAlbumsImplementationUid);
           // The target collection has also to be appeneded with the the relation id.
           // appending another level into the albums to get favourites and 1 is the relation id of albums
           targetCollection->AppendL( TMPXItemId(KGlxCollectionFavoritesId) );
           targetCollection->Set( 0 );
    
           CMPXCollectionPath* sourceItems = aMediaList.PathLC( NGlxListDefs::EPathFocusOrSelection );
      
           command = TGlxCommandFactory::AddToContainerCommandLC(
               *sourceItems, *targetCollection);
           CleanupStack::Pop(command); 
           
           CleanupStack::PopAndDestroy(sourceItems);
           CleanupStack::PopAndDestroy(targetCollection);
        }
    else
        {

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
    
            GlxAlbumSelectionPopup popupWidget;
            QModelIndexList modelList = popupWidget.GetSelectionList(albumMediaModel, &ok);
            if(ok && modelList.count() == 0)
            {
                createNewMedia();
                ok = false;        
            }    
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
        }
    OstTraceFunctionExit0( GLXCOMMANDHANDLERADDTOCONTAINER_CREATECOMMANDL_EXIT );
    return command;
    }
  
void GlxCommandHandlerAddToContainer::createNewMedia() const
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
