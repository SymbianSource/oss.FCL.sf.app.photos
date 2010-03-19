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
#include <glxviewids.h>
#include <glxmenumanager.h>
#include <QDebug>
#include <hbaction.h>
#include "hbmenu.h"
#include <glxcommandhandlers.hrh>
#include <QDebug>

GlxMenuManager::GlxMenuManager()
{
}
GlxMenuManager::~GlxMenuManager()
{
}
void GlxMenuManager::CreateViewMenu(qint32 viewId,HbMenu* menu,bool empty, int subState)
{
	Q_UNUSED(empty);
    switch(viewId)
    {
    case GLX_GRIDVIEW_ID:   {
                            qDebug()<<"GRIDVIEW MENU CREATED";
                            CreateGridMenu(menu);
                            }
                            break;

    case GLX_LISTVIEW_ID:   {
                            qDebug()<<"LISTVIEW MENU CREATED";
                            CreateListMenu(menu);
                            }
                            break;
    case GLX_FULLSCREENVIEW_ID:{
								//image viewer state 
                                if(2 == subState){
                                    CreateImageViewerMenu(menu);
                                }
                                else{
                                    qDebug()<<"FULLSCREENVIEW MENU CREATED";
                                    CreateFullscreenMenu(menu);
                                }
                               }
                            break;
							
    case GLX_DETAILSVIEW_ID: {
	//will create details specific menu later
                            qDebug()<<"DETAILS VIEW MENU CREATE";
                             }  
	                          
    case GLX_SLIDESHOWVIEW_ID :
        break;                            
                            
    default:                
                            break;

    }
}

void GlxMenuManager::createMarkingModeMenu(HbMenu* menu)
{
    qDebug()<< "GlxMenuManager::CreateMarkingModeMenu" ;  
    HbAction *action = NULL;
    
    action = menu->addAction("Mark All");
    action->setData(EGlxCmdMarkAll);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    
    action = menu->addAction("UnMark All");
    action->setData(EGlxCmdUnMarkAll);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    
    action = menu->addAction("Help");
    action->setData(EGlxCmdUnMarkAll);
    action->setVisible(FALSE);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    menu->setVisible(FALSE);
    
    action = menu->addAction("Exit");
    action->setData(EGlxCmdUnMarkAll);
    action->setVisible(FALSE);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    menu->setVisible(FALSE);
}

void GlxMenuManager::CreateGridMenu(HbMenu* menu)
{
    qDebug()<<"GlxMenuManager::CreateGridMenu";
    HbAction *action = NULL;
    action = menu->addAction("Send");
    action->setData(EGlxCmdSend);
    // action->setVisible(EFalse);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
   
    /*action = menu->addAction("Upload to web");
    action->setData(EGlxCmdUpload);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));*/
    action = menu->addAction("Slide Show");
    action->setData(EGlxCmdFirstSlideshow);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    action = menu->addAction("Add to album");
    action->setData(EGlxCmdAddToAlbum);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    /*action = menu->addAction("Sort by");
    //action->setData();
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    action = menu->addAction("Setting");
    //action->setData();
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));*/
    action = menu->addAction("Delete");
    action->setData(EGlxCmdDelete);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    /*action = menu->addAction("Help");
    //action->setData();
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    action = menu->addAction("Exit");*/
}

void GlxMenuManager::CreateListMenu(HbMenu* menu)
{
    qDebug()<<"GlxMenuManager::CreateListMenu";
    /*HbAction *action = menu->addAction("Setting");
    //action->setData();
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    action = menu->addAction("Help");
    //action->setData();
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    action = menu->addAction("Exit");
    //action->setData();
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));*/
}

void GlxMenuManager::CreateFullscreenMenu(HbMenu* menu)
{
    qDebug()<<"GlxMenuManager::CreateFullscreenMenu";
    HbAction *action = NULL;
    action = menu->addAction("Send");
    action->setData(EGlxCmdSend);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    action = menu->addAction("Slide Show");
    action->setData(EGlxCmdSelectSlideshow);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    action = menu->addAction("Add to album");
    action->setData(EGlxCmdAddToAlbum);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
	action = menu->addAction("Rotate");
    action->setData(EGlxCmdRotate);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    /*action = menu->addAction("Upload to web");
    action->setData(EGlxCmdUpload);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    action = menu->addAction("Use Image");
    //action->setData();
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    action = menu->addAction("Print");
    //action->setData();
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    action = menu->addAction("Edit");
    //action->setData();
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    action = menu->addAction("Details");
    action->setData(EGlxCmdDetailsOption);
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    action = menu->addAction("Help");
    //action->setData();
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
    action = menu->addAction("Exit");
    //action->setData();
    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));*/
}

void GlxMenuManager::CreateImageViewerMenu(HbMenu* menu)
{
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
    HbMenu *mainMenu = new HbMenu();
    HbAction *action = NULL;
	switch ( viewId ) {
	    case GLX_GRIDVIEW_ID :
	        action = mainMenu->addAction("Send");
	        action->setData(EGlxCmdSend);
	        connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
	        
	        action = mainMenu->addAction("Slide Show");
	        action->setData(EGlxCmdSelectSlideshow);
	        connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
		    
	        action = mainMenu->addAction("Add to album");
		    action->setData(EGlxCmdContextAddToAlbum);
		    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
		    
		    action = mainMenu->addAction("Delete");
		    action->setData(EGlxCmdContextDelete);
		    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
			break;
	    	
	    case GLX_LISTVIEW_ID :
	        action = mainMenu->addAction("Slide Show");
	        action->setData(EGlxCmdAlbumSlideShow);
	        connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));
	                    
		    /*action = mainMenu->addAction("Delete");
		    action->setData(EGlxCmdContextAlbumDelete);
		    connect(action, SIGNAL(triggered()), this, SLOT(menuItemSelected()));*/
			break;
		default:
		    break;	
		}

    mainMenu->exec(pos);

    delete mainMenu;

}
