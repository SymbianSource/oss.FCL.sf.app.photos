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
* Description:   ?Description
*
*/


#include <hbiconitem.h>
#include <hbframeitem.h>
#include <glxdetailscustomicon.h>
#include <QEvent>

//--------------------------------------------------------------------------------------------------------------------------------------------
//mousePressEvent
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsCustomIcon::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//clearCurrentModel
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsCustomIcon::mouseReleaseEvent (QGraphicsSceneMouseEvent *event) 
{
  Q_UNUSED(event)  
  emit updateFavourites();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//GlxDetailsCustomIcon
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsCustomIcon::GlxDetailsCustomIcon(QGraphicsItem *parent) : HbWidget(parent)
{
  mFavIcon = new HbIconItem(this);
  HbFrameItem* frame = new HbFrameItem(this);
  frame->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
  frame->frameDrawer().setFrameGraphicsName("qtg_fr_multimedia_trans");
  frame->graphicsItem()->setOpacity(0.2); 
  setBackgroundItem(frame->graphicsItem(),-1);
  
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//~GlxDetailsCustomIcon
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsCustomIcon::~GlxDetailsCustomIcon()
{
  delete mFavIcon;
  mFavIcon = NULL;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//setItemGeometry
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsCustomIcon::setItemGeometry(QRect screenRect) 
{
     setGeometry(screenRect);    
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//setItemSize
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsCustomIcon::setItemSize(const QSizeF &size) 
{
     mFavIcon->setSize(size);
} 

//--------------------------------------------------------------------------------------------------------------------------------------------
//setItemPos
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsCustomIcon::setItemPos(const qreal ax,const qreal ay)
{
    //place the FavIcon with respect to the parent i.e HbWidget 
    mFavIcon->setPos(ax ,ay);    
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//setItemIcon
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsCustomIcon::setItemIcon(const HbIcon &icon)
{
    mFavIcon->setIcon(icon);    
} 
