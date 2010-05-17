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


#include <glxeffectengine.h>
#include "glxeffectpluginbase.h"
#include "glxforwardtransitionplugin.h"
#include "glxbackwardtransitionplugin.h"
#include "glxfadeplugin.h"
#include <QDebug>

#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <QStringList>
const TUint32 KGlxTransitionEffect     = 0x1;
const TUint32 KGlxTransitionDelay  = 0x2;
const TUint32 KGlxSlow  = 0x3;
const TUint32 KGlxMeduim  = 0x4;
const TUint32 KGlxFast  = 0x5;
const TUint32 KGlxWaveEffect  = 0x6;
const TUint32 KGlxFadeEffect  = 0x7;
const TUint32 KGlxZoomToFaceEffect  = 0x8;
const TUint32 KCRUidGallery = 0x20007194;
GlxSlideShowSetting::GlxSlideShowSetting( int slideDelayTime, GlxEffect effect, GlxSlideShowMoveDir moveDir ) 
    : mSlideDelayTime(slideDelayTime),
      mEffect ( effect),
      mMoveDir ( moveDir)
{
    mSettingsManager = new XQSettingsManager();
    mTransitionEffectCenrepKey= new XQSettingsKey(XQSettingsKey::TargetCentralRepository, KCRUidGallery , KGlxTransitionEffect);
    mTransitionDelayCenrepKey= new XQSettingsKey(XQSettingsKey::TargetCentralRepository, KCRUidGallery , KGlxTransitionDelay);
    mSlowCenRepKey= new XQSettingsKey(XQSettingsKey::TargetCentralRepository, KCRUidGallery ,KGlxSlow);
    mMediumCenRepKey= new XQSettingsKey(XQSettingsKey::TargetCentralRepository, KCRUidGallery ,KGlxMeduim);
    mFastCenRepKey= new XQSettingsKey(XQSettingsKey::TargetCentralRepository, KCRUidGallery ,KGlxFast);

    mWaveEffectCenRepKey= new XQSettingsKey(XQSettingsKey::TargetCentralRepository, KCRUidGallery ,KGlxWaveEffect );
    mFadeEffectCenRepKey= new XQSettingsKey(XQSettingsKey::TargetCentralRepository, KCRUidGallery ,KGlxFadeEffect);
    mZoomEffectCenRepKey= new XQSettingsKey(XQSettingsKey::TargetCentralRepository, KCRUidGallery , KGlxZoomToFaceEffect );
}

GlxSlideShowSetting::~GlxSlideShowSetting( )
{
        delete mFastCenRepKey;
        delete mMediumCenRepKey;
        delete mSlowCenRepKey;
        delete mZoomEffectCenRepKey;
        delete mFadeEffectCenRepKey;
        delete mWaveEffectCenRepKey;
        delete mTransitionDelayCenrepKey;
        delete mTransitionEffectCenrepKey;
        delete mSettingsManager;
}
int GlxSlideShowSetting::slideShowDelayIndex()
{
    QVariant effectvalue = mSettingsManager->readItemValue(*mTransitionDelayCenrepKey);
    return effectvalue.toInt();
}
void GlxSlideShowSetting::setSlideShowDelayIndex( int index )
{
    mSettingsManager->writeItemValue(*mTransitionDelayCenrepKey, index);
}
QStringList GlxSlideShowSetting::slideShowEffectList()
{
	if(mSettingsManager->readItemValue(*mWaveEffectCenRepKey).toInt() == WAVE_EFFECT)
        mEffectList<<"Flip";
    if(mSettingsManager->readItemValue(*mFadeEffectCenRepKey).toInt() == SMOOTH_FADE)
        mEffectList<<"Fade";
    if(mSettingsManager->readItemValue(*mZoomEffectCenRepKey).toInt() == ZOOM_TO_FACE)
        mEffectList<<"Zoom to face"; 
    return mEffectList;
}
int GlxSlideShowSetting::slideShowEffectIndex()
{
    QVariant effectvalue = mSettingsManager->readItemValue(*mTransitionEffectCenrepKey);
    return effectvalue.toInt();
}
void GlxSlideShowSetting::setslideShowEffectIndex( int index )
{
    mEffect = ( GlxEffect ) ( index + 1 ) ;
    mSettingsManager->writeItemValue(*mTransitionEffectCenrepKey, index);
}
void GlxSlideShowSetting::readSlideShowSetting()
{
//To:Do read from the file system
    QVariant effectvalue = mSettingsManager->readItemValue(*mTransitionDelayCenrepKey);
    switch (effectvalue.toInt()) {
               case SLOW:
                   mSlideDelayTime = mSettingsManager->readItemValue(*mSlowCenRepKey).toInt();
                   break;
               case MEDIUM:
                   mSlideDelayTime = mSettingsManager->readItemValue(*mMediumCenRepKey).toInt();
                   break;
               case FAST:
                   mSlideDelayTime = mSettingsManager->readItemValue(*mFastCenRepKey).toInt();
                   break;
               default:
    				mSlideDelayTime = 3000;
                   break;
	}
    mEffect = ( GlxEffect ) ( mSettingsManager->readItemValue(*mTransitionEffectCenrepKey).toInt() + 1 )  ; 
    mMoveDir = MOVE_FORWARD;
    qDebug("GlxSlideShowSetting::readSlideShowSetting() slide delay time %d effect %d move direction %d", mSlideDelayTime, mEffect, mMoveDir);
}


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
            mEffectFileList.append( QString(":/data/gridtoalbumlisthide.fxml"));
            mItemType.append( QString("HbView") );
            mEventType.append(QString("click4") );
                        
            mEffectFileList.append( QString(":/data/gridtoalbumlist.fxml"));
            mItemType.append( QString("HbListView") );
            mEventType.append(QString("click5") );
            break;
             
        case ALBUMLIST_TO_GRID:
            mEffectFileList.append( QString(":/data/albumlisttogrid.fxml"));
            mItemType.append( QString("HbListView") );
            mEventType.append(QString("click6") );
                        
            mEffectFileList.append( QString(":/data/albumlisttogridshow.fxml"));
            mItemType.append( QString("HbView") );
            mEventType.append(QString("click7") );
            break;
            
        case FULLSCREEN_TO_DETAIL :
            mEffectFileList.append( QString(":/data/view_flip_hide.fxml"));
            mItemType.append( QString("HbView") );
            mEventType.append(QString("click8") );
                        
            mEffectFileList.append( QString(":/data/view_flip_show.fxml"));
            mItemType.append( QString("HbView") );
            mEventType.append(QString("click9") );
            mTransitionLater = true;
            break;
            
        case DETAIL_TO_FULLSCREEN :
            mEffectFileList.append( QString(":/data/view_flip_hide.fxml"));
            mItemType.append( QString("HbView") );
            mEventType.append(QString("click10") );
                        
            mEffectFileList.append( QString(":/data/view_flip_show.fxml"));
            mItemType.append( QString("HbView") );
            mEventType.append(QString("click11") );
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


GlxSlideShowEffectEngine::GlxSlideShowEffectEngine( ) : mNbrEffectRunning( 0 ), 
        mEffectPlugin(NULL), 
        mTransitionEffect(NO_EFFECT)
{
    qDebug("GlxSlideShowEffectEngine::GlxSlideShowEffectEngine()");
    mTransitionEffectList.clear();
}

GlxSlideShowEffectEngine::~GlxSlideShowEffectEngine()
{
    qDebug("GlxSlideShowEffectEngine::~GlxSlideShowEffectEngine()");
    delete mEffectPlugin;
    mEffectPlugin = NULL;
	
    cleanTransitionEfffect();
}

void GlxSlideShowEffectEngine::registerEffect(const QString &itemType)
{
    effectPluginResolver();
    QList <QString > effectPathList = mEffectPlugin->getEffectFileList();
    
    qDebug("GlxSlideShowEffectEngine::registerEffect() item type %s file path %s", itemType.utf16(), effectPathList[0].utf16());
    for ( int i = 0; i < effectPathList.count() ; ++i ) {
        HbEffect::add(itemType, effectPathList.at(i), QString( "Click%1" ).arg(i));
    }    
}

void GlxSlideShowEffectEngine::deRegisterEffect(const QString &itemType)
{
    qDebug("GlxSlideShowEffectEngine::deRegisterEffect() item type %s", itemType.utf16());
    QList <QString > effectPathList = mEffectPlugin->getEffectFileList();
    for ( int i = 0; i < effectPathList.count() ; ++i ) {
        HbEffect::remove(itemType, effectPathList.at(i), QString( "Click%1" ).arg(i));
    }
    delete mEffectPlugin ;
    mEffectPlugin = NULL;
}

void GlxSlideShowEffectEngine::registerTransitionEffect()  
{
    qDebug("GlxSlideShowEffectEngine::registerTransitionEffect()");
    initTransitionEffect();
}

void GlxSlideShowEffectEngine::deregistertransitionEffect()
{
    qDebug("GlxSlideShowEffectEngine::deregisterTransitionEffect()");
    cleanTransitionEfffect();
}

void GlxSlideShowEffectEngine::runEffect(QGraphicsItem *  item, const QString &  itemType )
{
    qDebug("GlxSlideShowEffectEngine::runEffect()1 item type %s", itemType.utf16());
    HbEffect::start(item, itemType, QString( "Click1" ), this, "slideShowEffectFinished");
    ++mNbrEffectRunning;
}

void GlxSlideShowEffectEngine::runEffect(QList< QGraphicsItem * > &  items, const QString &  itemType )
{
    qDebug("GlxSlideShowEffectEngine::runEffect()2 item Type %s", itemType.utf16());
    mEffectPlugin->setUpItems(items);
    for ( int i = 0; i < items.count() ; ++i ) {
        if ( mEffectPlugin->isAnimationLater(i) == FALSE ) {
            HbEffect::start(items.at(i), itemType, QString( "Click%1").arg(i), this, "slideShowEffectFinished");
        }
        ++mNbrEffectRunning;
    }
}

void GlxSlideShowEffectEngine::runEffect(QList< QGraphicsItem * > &  items, GlxEffect transitionEffect)
{
    qDebug("GlxSlideShowEffectEngine::runEffect()3 effect type %d ", transitionEffect);
    
    GlxTransitionEffectSetting *effectSetting = mTransitionEffectList.value( transitionEffect );
    
    if ( effectSetting == NULL || items.count() != effectSetting->count() ) {
         return;
    }
    
    mTransitionEffect = transitionEffect;
    for ( int i = 0; i < effectSetting->count() ; ++i) {
        ++mNbrEffectRunning;
        if ( ( i == effectSetting->count() -1) && effectSetting->isTransitionLater() )
        {
            effectSetting->setAnimationItem( items.at(i) );
            items.at(i)->hide();
        }
        else {
            HbEffect::start(items.at(i), effectSetting->itemType().at(i), effectSetting->eventType().at(i), this, "transitionEffectFinished");
        }
    }    
}

void GlxSlideShowEffectEngine::cancelEffect(QGraphicsItem *  item)
{
    if ( HbEffect::effectRunning( item, QString( "Click1" ) ) ) {
        HbEffect::cancel( item, QString( "Click1" ) );
    }
}

void GlxSlideShowEffectEngine::cancelEffect(const QList< QGraphicsItem * > &  items)
{
    for ( int i = 0; i < items.count() ; ++i ) {    
        if ( HbEffect::effectRunning( items.at(i), QString( "Click%1").arg(i) ) ) {
            HbEffect::cancel( items.at(i), QString( "Click%1").arg(i) );
        }
    }
}

void GlxSlideShowEffectEngine::cancelEffect(QList< QGraphicsItem * > &  items, GlxEffect transitionEffect)
{
    GlxTransitionEffectSetting *effectSetting = mTransitionEffectList.value( transitionEffect );
    
    if ( effectSetting == NULL || items.count() != effectSetting->count() ) {
         return;
    }
    
    for ( int i = 0; i < effectSetting->count() ; ++i) {
        HbEffect::cancel(items.at(i), effectSetting->eventType().at(i) );
    }     
}

bool GlxSlideShowEffectEngine::isEffectRuning(QGraphicsItem *  item)
{
    if ( HbEffect::effectRunning( item, QString( "Click1" ) ) ) {
        return true;
    }
    return false;
}

bool GlxSlideShowEffectEngine::isEffectRuning(const QList< QGraphicsItem * > &  items)
{
    for ( int i = 0; i < items.count() ; ++i ) {    
        if ( HbEffect::effectRunning( items.at(i), QString( "Click%1").arg(i) ) ) {
            return true;
        }
    }
    return false;
}

void GlxSlideShowEffectEngine::slideShowEffectFinished( const HbEffect::EffectStatus &status )
{
    Q_UNUSED( status )
    qDebug("GlxSlideShowEffectEngine::slideShowEffectFinished() number of effect %d ", mNbrEffectRunning);
    
    --mNbrEffectRunning;
    
    if ( mEffectPlugin->isAnimationLater( mNbrEffectRunning) ) {
        HbEffect::start( mEffectPlugin->animationItem(), mEffectPlugin->ItemType(), QString( "Click%1").arg(mNbrEffectRunning), this, "slideShowEffectFinished");
    }
    
    if (mNbrEffectRunning == 0) {
        emit effectFinished();
    }
}

void GlxSlideShowEffectEngine::transitionEffectFinished( const HbEffect::EffectStatus &status )
{
    Q_UNUSED( status )
    qDebug("GlxSlideShowEffectEngine::transitionEffectFinished() number of effect %d status %d", mNbrEffectRunning, status.reason);
    
    --mNbrEffectRunning;
    if ( mNbrEffectRunning == 1 ) {
        GlxTransitionEffectSetting *effectSetting = mTransitionEffectList.value( mTransitionEffect );
        if (  effectSetting->isTransitionLater() ){ 
            effectSetting->animationItem()->show();
            HbEffect::start( effectSetting->animationItem(), effectSetting->itemType().at(1), effectSetting->eventType().at(1), this, "transitionEffectFinished");
            mTransitionEffect = NO_EFFECT;
        }    
    }
    
    if (mNbrEffectRunning == 0) {
        emit effectFinished();
    }
}

void GlxSlideShowEffectEngine::effectPluginResolver()
{
//TO:DO improved the code by using factory design pattern

    delete mEffectPlugin;
    mEffectPlugin = NULL;
    
    switch ( mSlideShowSetting.effect() ) {
    case WAVE_EFFECT :
        mEffectPlugin = new GlxForwardTransitionPlugin();
        break ;
        
    /*case TRANSITION_EFFECT :
        if ( slideShowMoveDir() == MOVE_FORWARD ) {
            mEffectPlugin = new GlxForwardTransitionPlugin();
        }
        else {
            mEffectPlugin = new GlxBackwardTransitionPlugin();
        }
        break;*/
        
    case SMOOTH_FADE :
        mEffectPlugin = new GlxFadePlugin();
        break;
        
    default :
        mEffectPlugin = new GlxFadePlugin();
        break;        
    }
}

void GlxSlideShowEffectEngine::initTransitionEffect()
{
    GlxTransitionEffectSetting *effectSetting = NULL;
    
    effectSetting = new GlxTransitionEffectSetting(GRID_TO_FULLSCREEN);
    for ( int i = 0; i < effectSetting->count(); ++i ) {
        HbEffect::add( effectSetting->itemType().at(i), effectSetting->effectFileList().at(i), effectSetting->eventType().at(i)) ;
    }
    mTransitionEffectList[GRID_TO_FULLSCREEN] = effectSetting;
    
    effectSetting = new GlxTransitionEffectSetting( FULLSCREEN_TO_GRID );
    for ( int i = 0; i < effectSetting->count(); ++i ) {
        HbEffect::add( effectSetting->itemType().at(i), effectSetting->effectFileList().at(i), effectSetting->eventType().at(i)) ;
    }
    mTransitionEffectList[FULLSCREEN_TO_GRID] = effectSetting;
    
    effectSetting = new GlxTransitionEffectSetting( GRID_TO_ALBUMLIST );
    for ( int i = 0; i < effectSetting->count(); ++i ) {
        HbEffect::add( effectSetting->itemType().at(i), effectSetting->effectFileList().at(i), effectSetting->eventType().at(i)) ;
    }
    mTransitionEffectList[GRID_TO_ALBUMLIST] = effectSetting;
    
    effectSetting = new GlxTransitionEffectSetting( ALBUMLIST_TO_GRID );
    for ( int i = 0; i < effectSetting->count(); ++i ) {
        HbEffect::add( effectSetting->itemType().at(i), effectSetting->effectFileList().at(i), effectSetting->eventType().at(i)) ;
    }
    mTransitionEffectList[ALBUMLIST_TO_GRID] = effectSetting;
    
    effectSetting = new GlxTransitionEffectSetting( FULLSCREEN_TO_DETAIL );
    for ( int i = 0; i < effectSetting->count(); ++i ) {
        HbEffect::add( effectSetting->itemType().at(i), effectSetting->effectFileList().at(i), effectSetting->eventType().at(i)) ;
    }
    mTransitionEffectList[FULLSCREEN_TO_DETAIL] = effectSetting; 
    
    effectSetting = new GlxTransitionEffectSetting( DETAIL_TO_FULLSCREEN );
    for ( int i = 0; i < effectSetting->count(); ++i ) {
        HbEffect::add( effectSetting->itemType().at(i), effectSetting->effectFileList().at(i), effectSetting->eventType().at(i)) ;
    }
    mTransitionEffectList[DETAIL_TO_FULLSCREEN] = effectSetting;    
}

void GlxSlideShowEffectEngine::cleanTransitionEfffect()
{	
    qDebug("GlxSlideShowEffectEngine::cleanTrnastionEfffect()");
    
    foreach( GlxTransitionEffectSetting *list, mTransitionEffectList) {
        for ( int i = 0; i < list->count(); ++i ) {
            HbEffect::remove( list->itemType().at(i), list->effectFileList().at(i), list->eventType().at(i)) ;
        }
        delete list;
    }
    mTransitionEffectList.clear();	
    
}

