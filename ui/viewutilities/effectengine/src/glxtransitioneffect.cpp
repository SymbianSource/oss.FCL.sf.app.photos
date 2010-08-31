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

#include <glxtransitioneffect.h>

#include <QString>

GlxTransitionEffectSetting::GlxTransitionEffectSetting(GlxEffect effect): mEffect(effect), mTransitionLater(false), mItem(0)
{
    qDebug("GlxTransitionEffectSetting::GlxTransitionEffectSetting() effect id %d ", effect);
    mEffectFileList.clear();
    mItemType.clear();
    mEventType.clear();
    init();
}

void GlxTransitionEffectSetting::init()
{
    qDebug("GlxTransitionEffectSetting::init() effect id %d ", mEffect);
    
    switch( mEffect ) {
        case GRID_TO_FULLSCREEN :
            mEffectFileList.append( QString(":/data/gridtofullscreenhide.fxml"));
            mItemType.append( QString("HbGridViewItem") );
            mEventType.append(QString("click1") );
            
            mEffectFileList.append( QString(":/data/gridtofullscreenshow.fxml"));
            mItemType.append( QString("HbView") );
            mEventType.append(QString("click2") );
            break;
            
        case FULLSCREEN_TO_GRID :
            mEffectFileList.append( QString(":/data/fullscreentogrid.fxml"));
            mItemType.append( QString("HbGridView") );
            mEventType.append(QString("click3") );
            break;
            
        case GRID_TO_ALBUMLIST:
            mEffectFileList.append( QString( "view_hide_normal" ) );
            mItemType.append( QString( "HbView" ) );
            mEventType.append(QString( "click4" ) );
                        
            mEffectFileList.append( QString( "view_show_normal" ) );
            mItemType.append( QString( "HbListView" ) );
            mEventType.append( QString( "click5" ) );
            break;
             
        case ALBUMLIST_TO_GRID:
            mEffectFileList.append( QString( "view_hide_back" ) );
            mItemType.append( QString( "HbListView" ) );
            mEventType.append( QString( "click6" ) );
                        
            mEffectFileList.append( QString( "view_show_back" ) );
            mItemType.append( QString( "HbView" ) );
            mEventType.append( QString( "click7" ) );
            break;
            
        case FULLSCREEN_TO_DETAIL :
            mEffectFileList.append( QString( "view_hide_normal_alt" ) );
            mItemType.append( QString( "HbView" ) );
            mEventType.append( QString( "click8" ) );
                        
            mEffectFileList.append( QString( "view_show_normal_alt" ) );
            mItemType.append( QString( "HbView" ) );
            mEventType.append(QString( "click9" ) );
            mTransitionLater = true;
            break;
            
        case DETAIL_TO_FULLSCREEN :
            mEffectFileList.append( "view_hide_back_alt" );
            mItemType.append( QString( "HbView" ) );
            mEventType.append( QString( "click10" ) );
                        
            mEffectFileList.append( "view_show_back_alt" );
            mItemType.append( QString( "HbView" ) );
            mEventType.append( QString( "click11" ) );
            mTransitionLater = true;
            break;            
            
        default :
            break;
    }    
}

GlxTransitionEffectSetting::~GlxTransitionEffectSetting()
{
    qDebug("GlxTransitionEffectSetting::~GlxTransitionEffectSetting() effect id %d ", mEffect);
    mEffectFileList.clear();
    mItemType.clear();
    mEventType.clear();
}

