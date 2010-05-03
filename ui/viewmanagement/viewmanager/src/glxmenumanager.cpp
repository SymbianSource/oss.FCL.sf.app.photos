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

//include
#include <QDebug>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbmainwindow.h>

//user include
#include "glxviewids.h"
#include "glxmenumanager.h"
#include "glxcommandhandlers.hrh"
#include "glxmodelparm.h"
#include "glxlocalisationstrings.h"



GlxMenuManager::GlxMenuManager(HbMainWindow* mainWindow)
 : mMainWindow( mainWindow ),
   mContextMenu( 0 )
{
}

GlxMenuManager::~GlxMenuManager()
{
}

void GlxMenuManager::createMarkingModeMenu(HbMenu* menu)
{
    qDebug()<< "GlxMenuManager::CreateMarkingModeMenu" ;  
    HbAction *action = NULL;
    
    action = menu->addAction(GLX_OPTION_MARK_ALL);
    action->setData(EGlxCmdMarkAll);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    
    action = menu->addAction(GLX_OPTION_UN_MARK_ALL);
    action->setDisabled(true);  //Dim UnMarkAll when no images are marked
    action->setData(EGlxCmdUnMarkAll);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));    
}

void GlxMenuManager::disableAction(HbMenu* menu, bool disable)
{
    QList<QAction*> actionList = menu->actions();
    for ( int i = 0 ; i < actionList.count(); i++) {
        if(actionList.at(i)->data()==EGlxCmdUnMarkAll) {
               actionList.at(i)->setDisabled(disable);
               break;
        }
     }
}
void GlxMenuManager::addMenu(qint32 viewId, HbMenu* menu)
{
    switch(viewId) {
    case GLX_GRIDVIEW_ID:
        connect( menu, SIGNAL( aboutToShow() ), this, SLOT( updateGridMenu() ) );
        CreateGridMenu( menu );
        break;                            

    case GLX_LISTVIEW_ID: 
        CreateListMenu ( menu );
        break;
        
    case GLX_FULLSCREENVIEW_ID:
        connect( menu, SIGNAL( aboutToShow() ), this, SLOT( updateFullscreenMenu() ) );
        CreateFullscreenMenu( menu );
        break;
                
    default:
        break;
    }  
}

void GlxMenuManager::removeMenu(qint32 viewId, HbMenu* menu)
{
    switch(viewId) {
    case GLX_GRIDVIEW_ID:
        disconnect( menu, SIGNAL( aboutToShow() ), this, SLOT( updateGridMenu() ) );
        break;                            
        
    case GLX_FULLSCREENVIEW_ID:
        disconnect( menu, SIGNAL( aboutToShow() ), this, SLOT( updateFullscreenMenu() ) );
        break;
                
    default:
        break;
    }  
}

void GlxMenuManager::CreateGridMenu(HbMenu* menu)
{
    qDebug()<<"GlxMenuManager::CreateGridMenu";
    HbAction *action = NULL;
    
    action = menu->addAction(GLX_OPTION_SHARE);
    action->setData(EGlxCmdSend);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
   
    mSubMenu = menu->addMenu(GLX_OPTION_SLIDESHOW);
    action = mSubMenu->addAction(GLX_OPTION_SS_PLAY); 
    action->setData(EGlxCmdFirstSlideshow);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    
    action = mSubMenu->addAction(GLX_OPTION_SS_SETTINGS);
    action->setData(EGlxCmdSlideshowSettings);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected())); 
    action = menu->addAction(GLX_OPTION_ADD_TO_ALBUM);
    action->setData(EGlxCmdAddToAlbum);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    
    action = menu->addAction(GLX_OPTION_REMOVE_FROM_ALBUM);
    action->setData(EGlxCmdRemoveFrom);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));

    action = menu->addAction(GLX_OPTION_DELETE);
    action->setData(EGlxCmdDelete);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
}

void GlxMenuManager::CreateListMenu(HbMenu* menu)
{
    qDebug()<<"GlxMenuManager::CreateListMenu";
    HbAction *action = NULL;
    
    action = menu->addAction(GLX_OPTION_NEW_ALBUM);
    action->setData(EGlxCmdAddMedia);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
}

void GlxMenuManager::CreateFullscreenMenu(HbMenu* menu)
{
    qDebug()<<"GlxMenuManager::CreateFullscreenMenu";
    HbAction *action = NULL;
    
    action = menu->addAction(GLX_OPTION_SHARE);
    action->setData(EGlxCmdSend);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    
    mSubMenu = menu->addMenu(GLX_OPTION_SLIDESHOW);
    action = mSubMenu->addAction(GLX_OPTION_SS_PLAY); 
    action->setData(EGlxCmdSelectSlideshow);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected())); 
    action = mSubMenu->addAction(GLX_OPTION_SS_SETTINGS);
    action->setData(EGlxCmdSlideshowSettings);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    
    action = menu->addAction(GLX_OPTION_ADD_TO_ALBUM);
    action->setData(EGlxCmdAddToAlbum);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
	
    action = menu->addAction("Rotate");
    action->setData(EGlxCmdRotate);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
}

void GlxMenuManager::setAllActionVisibility( QList<QAction*> actionList, bool visible )
{
    qDebug() << "GlxMenuManager::setAllActionVisibility count " << actionList.count() << " visible" << visible;
    for ( int i = 0 ; i < actionList.count(); i++) {
        actionList.at(i)->setVisible(visible);
    }
}

int GlxMenuManager::viewSubState()
{
    QVariant variant = mModel->data( mModel->index(0,0), GlxSubStateRole );    
    if ( variant.isValid() &&  variant.canConvert<int> () ) {
        return variant.value<int>() ;
    }
    return -1;
}

void GlxMenuManager::updateGridMenu()
{
    qDebug() << "GlxMenuManager::updateGridMenu";

    HbMenu *menu = qobject_cast<HbMenu*>( sender() );
    static bool isAllActionDisable = FALSE;
    int rowCount = mModel->rowCount();    
    QList<QAction*> actionList = menu->actions();
    
    if ( rowCount == 0 && isAllActionDisable == FALSE) {
        qDebug() << "GlxMenuManager::updateGridMenu set all visible FALSE";
        setAllActionVisibility( actionList, FALSE );
        isAllActionDisable = TRUE;
    }    
    
    if ( rowCount && isAllActionDisable == TRUE ) {
        setAllActionVisibility( actionList, TRUE );
        isAllActionDisable = FALSE;
        qDebug() << "GlxMenuManager::updateGridMenu set all visible TRUE" ;
    }
    
    if ( rowCount ) {
        int state =  viewSubState();
        
        switch ( state ) {
        case ALBUM_ITEM_S :
            actionList.at(GlxGridViewRemoveFromAlbum)->setVisible( TRUE );
            break ;
            
        default :    
            actionList.at(GlxGridViewRemoveFromAlbum)->setVisible( FALSE );
            break ;
        }       
    }    
}

void GlxMenuManager::updateFullscreenMenu()
{
    HbMenu *menu = qobject_cast<HbMenu*>( sender() );
    QList<QAction*> actionList = menu->actions();
    static bool isAllActionDisable = FALSE;
    int state = viewSubState() ;
    
    if ( state == IMAGEVIEWER_S &&  isAllActionDisable == FALSE ) {        
        setAllActionVisibility( actionList, FALSE );
        isAllActionDisable = TRUE;
    }
  
    if ( state != IMAGEVIEWER_S &&  isAllActionDisable == TRUE ) {
        setAllActionVisibility( actionList, TRUE );
        isAllActionDisable = FALSE;
    }    
}

void GlxMenuManager::menuItemSelected()
{
    HbAction *action = qobject_cast<HbAction*>(sender());
    
    qint32 commandId = action->data().toInt();
    emit commandTriggered(commandId);
}

void GlxMenuManager::ShowItemSpecificMenu(qint32 viewId,QPointF pos)
{
    qDebug("GlxMenuManager::showContextMenu " );
    mContextMenu = new HbMenu();
    HbAction *action = NULL;

    switch ( viewId ) {
	    case GLX_GRIDVIEW_ID :
	        action = mContextMenu->addAction(GLX_MENU_SHARE);
	        action->setData(EGlxCmdContextSend);
	        connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
	        
	        action = mContextMenu->addAction(GLX_MENU_SLIDESHOW);
	        action->setData(EGlxCmdSelectSlideshow);
	        connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
		    
	        action = mContextMenu->addAction(GLX_MENU_ADD_TO_ALBUM);
		    action->setData(EGlxCmdContextAddToAlbum);
		    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
		    
		    action = mContextMenu->addAction(GLX_MENU_DELETE);
		    action->setData(EGlxCmdContextDelete);
		    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
			break;
	    	
	    case GLX_LISTVIEW_ID :
	        action = mContextMenu->addAction(GLX_MENU_SLIDESHOW);
	        action->setData(EGlxCmdAlbumSlideShow);
	        connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    
            action = mContextMenu->addAction(GLX_MENU_RENAME);
            action->setData(EGlxCmdContextRename);
            connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
	                    
		    action = mContextMenu->addAction(GLX_MENU_DELETE);
		    action->setData(EGlxCmdContextAlbumDelete);
		    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
			break;
			
		default:
		    break;	
		}

	connect( mMainWindow, SIGNAL( aboutToChangeOrientation () ), mContextMenu, SLOT( close() ) );
	connect( mContextMenu, SIGNAL( aboutToClose () ), this, SLOT( closeContextMenu() ) );
	mContextMenu->setPreferredPos( pos );
	mContextMenu->show();
    
}

void GlxMenuManager::closeContextMenu()
{
    disconnect( mMainWindow, SIGNAL( aboutToChangeOrientation () ), mContextMenu, SLOT( close() ) );
    disconnect( mContextMenu, SIGNAL( aboutToClose () ), this, SLOT( closeContextMenu() ) ); 
    mContextMenu->deleteLater();
    mContextMenu = NULL;
}

