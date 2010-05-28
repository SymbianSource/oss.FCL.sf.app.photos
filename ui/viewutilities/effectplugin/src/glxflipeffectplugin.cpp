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


#include "glxflipeffectplugin.h"
#include "glxuistd.h"

GlxFlipEffectPlugin::GlxFlipEffectPlugin() : GlxEffectPluginBase( FLIP_EFFECT )
{
    mEffectFileList.append(QString(":/data/view_flip_hide.fxml"));
    mEffectFileList.append(QString(":/data/view_flip_show.fxml"));
}

void GlxFlipEffectPlugin::setUpItems( QList< QGraphicsItem * > &  items )
{
    if ( items.count() < 2 ) 
        return;
    
    mItem = items.at(1);
    mItem->hide();
    mItem->setPos(0,0);
    items.at(0)->setPos(0,0);
}

bool GlxFlipEffectPlugin::isAnimationLater(int index) 
{
    if ( index == 1) {
        return true;
    }
    return false;
}

QGraphicsItem * GlxFlipEffectPlugin::animationItem() 
{ 
    mItem->show();
    return mItem ; 
}

GlxFlipEffectPlugin::~GlxFlipEffectPlugin()
{
    mEffectFileList.clear();
}


