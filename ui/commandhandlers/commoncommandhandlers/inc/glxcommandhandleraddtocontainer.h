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

#ifndef GLXCOMMANDHANDLERADDTOCONTAINER_H
#define GLXCOMMANDHANDLERADDTOCONTAINER_H

#include <QModelIndexList>
#include <glxmpxcommandhandler.h>
#include <QGraphicsWidget>

#ifdef BUILD_COMMONCOMMANDHANDLERS
#define GLX_COMMONCOMMANDHANDLERS_EXPORT Q_DECL_EXPORT
#else
#define GLX_COMMONCOMMANDHANDLERS_EXPORT Q_DECL_IMPORT
#endif
class CMPXCollectionPath;
class HbPushButton;
class HbListView;
class QGraphicsGridLayout;
class GlxAlbumModel;
class QGraphicsItem;

class GLX_COMMONCOMMANDHANDLERS_EXPORT GlxCommandHandlerAddToContainer : public GlxMpxCommandHandler
	{
	Q_OBJECT    
public:
    GlxCommandHandlerAddToContainer();
    ~GlxCommandHandlerAddToContainer();
    CMPXCommand* CreateCommandL(TInt aCommandId, MGlxMediaList& aMediaList, TBool& aConsume) const ;
    QModelIndexList GetSelectionList(GlxAlbumModel *model,bool *ok = 0) const;

private:
    static TInt iSelectionCount;
    mutable bool mNewMediaAdded ;
    mutable CMPXCollectionPath* mTargetContainers ;

private slots:    
	void createNewMedia();
	};

class GlxQueryContentWidget :public QGraphicsWidget
{
	Q_OBJECT
public:
    HbPushButton* mButton;
    HbListView* mListView;
    QGraphicsGridLayout *mGrid;
    GlxQueryContentWidget(QGraphicsItem* parent = 0);
    ~GlxQueryContentWidget();
};
    
    
#endif // GLXCOMMANDHANDLERADDTOCONTAINER_H
