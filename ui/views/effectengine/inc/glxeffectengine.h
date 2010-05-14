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


#ifndef GLXEFFECTENGINE_H
#define GLXEFFECTENGINE_H

#ifdef BUILD_GLXVIEWS
#define GLX_VIEW_EXPORT Q_DECL_EXPORT
#else
#define GLX_VIEW_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <hbeffect.h>
#include <QString>
#include <QHash>
#include <QList>
#include "glxuistd.h"

class XQSettingsKey;
class XQSettingsManager;
class QStringList;
typedef enum 
{
    NO_MOVE,
    MOVE_FORWARD,
    MOVE_BACKWARD,
} GlxSlideShowMoveDir;

/*
 * This class is used for read the setting either from File System or from some xml file.
 * This Setting is used by both server and client, but It is coupled with the server and client retreive the data through the server
 */
class GlxSlideShowSetting
{
public :
    GlxSlideShowSetting(int slideDelayTime = 0, GlxEffect effect = NO_EFFECT, GlxSlideShowMoveDir moveDir = NO_MOVE);
    ~GlxSlideShowSetting();
    int slideDelayTime ( ) { return mSlideDelayTime; }
    GlxEffect effect() { return mEffect ; }
    void setEffect(GlxEffect effect) { mEffect = effect ;}
    GlxSlideShowMoveDir slideShowMoveDir() { return mMoveDir; }
    /* Returns the index to the selected transition effect
    This value is fetched from Central repository */
    int slideShowEffectIndex() ;
    /*SlideShowSettingsView will call this API to set the index of the transition effect chosen. 
    This data will be written to the Central Repository*/
     void setslideShowEffectIndex( int index );
    
    /* Returns the index to the selected transition delay /
    This value is fetched from Central repository */
    int slideShowDelayIndex();
    
    /*SlideShowSettingsView will call this API to set the index of the transition delay chosen. 
    This data will be written to the Central Repository*/
    void setSlideShowDelayIndex( int index );
    
    /*This will return the list of effects as available in the central repository */
    QStringList slideShowEffectList();
 
/*
 * This funtion read the user setting store in a file system.
 * 
 */	
    void readSlideShowSetting();
	
private :
    int mSlideDelayTime; //user set time interval in between showing slide
    GlxEffect mEffect;         //user defined effect, To:Do It is either some integer value or some enum
    GlxSlideShowDelay mDelay;
    GlxSlideShowMoveDir mMoveDir;	//user defined slide move direction
    QStringList mEffectList;
    XQSettingsManager *mSettingsManager;
    XQSettingsKey *mTransitionEffectCenrepKey;
    XQSettingsKey *mTransitionDelayCenrepKey;
    XQSettingsKey *mWaveEffectCenRepKey;
    XQSettingsKey *mFadeEffectCenRepKey;
    XQSettingsKey *mZoomEffectCenRepKey;
    XQSettingsKey *mSlowCenRepKey;
    XQSettingsKey *mMediumCenRepKey;
    XQSettingsKey *mFastCenRepKey;
};

/*
 * This class have the views transition effect information
 */
class GlxTransitionEffectSetting
{
public :
    GlxTransitionEffectSetting(GlxEffect effect);
    ~GlxTransitionEffectSetting();
    
    QList <QString > effectFileList() { return mEffectFileList ; }
    QList <QString > itemType() { return mItemType ; }
    QList <QString > eventType() { return mEventType ; }
    bool isTransitionLater() { return mTransitionLater ; }
    QGraphicsItem * animationItem() { return mItem ; }
    
    void setEffectFileList( QList <QString > & effectFileList) { mEffectFileList = effectFileList ; }
    void setItemTypes( QList <QString > & itemType) { mItemType = itemType; }
    void setEventTypes( QList <QString > & eventType) { mEventType = eventType ; }
    void setTransitionLater(bool transitionLater) { mTransitionLater = transitionLater ; }
    void setAnimationItem(QGraphicsItem *item) { mItem = item ;}
    int count() { return mEffectFileList.count() ; }
    
private :
    void init();
    
private :
    GlxEffect mEffect;     //transition effect type
    bool mTransitionLater; //second animation will run same time(false) or later (true)
    QGraphicsItem *mItem; //it will used in the case of animation run later
    QList <QString > mEffectFileList; //list of fxml file used for animation
    QList <QString > mItemType; //list of item type 
    QList <QString > mEventType; //list of event type
};


/*
 * It is customise class for Photo Application to run the effect.
 * In the case of effect setting required to read through File System: 
 * first read the setting, register the effect and then run the effect.
 * In the case of run the client effect: first Set the effect, register the effect and then run the effect.
 */
 
class GlxEffectPluginBase;

class GLX_VIEW_EXPORT GlxSlideShowEffectEngine : public QObject
{
    Q_OBJECT
    
public:
    GlxSlideShowEffectEngine( );
    ~GlxSlideShowEffectEngine();
    int slideDelayTime ( ) { return mSlideShowSetting.slideDelayTime(); }    
    GlxSlideShowMoveDir slideShowMoveDir() { return mSlideShowSetting.slideShowMoveDir(); }
    void readSetting() { mSlideShowSetting.readSlideShowSetting(); }
    void setEffect(GlxEffect effect) { mSlideShowSetting.setEffect( effect ); }
    
    void registerEffect(const QString &itemType);
    void deRegisterEffect(const QString &itemType);
    
/*  To register a set of transition effect 
 *  e.g "grid to full screeen", "fullscreen to grid"
 *  
 */  
    void registerTransitionEffect();    
    void deregistertransitionEffect();
    
    void runEffect(QGraphicsItem *  item, const QString &  itemType ); 
    void runEffect(QList< QGraphicsItem * > &  items, const QString &  itemType );
    void runEffect(QList< QGraphicsItem * > &  items, GlxEffect transitionEffect);
    
    void cancelEffect(QGraphicsItem *  item);
    void cancelEffect( const QList< QGraphicsItem * > &  items );
    void cancelEffect(QList< QGraphicsItem * > &  items, GlxEffect transitionEffect);
    
    bool isEffectRuning(QGraphicsItem *  item);
    bool isEffectRuning(const QList< QGraphicsItem * > &  items);
    
signals:
    void effectFinished();
    
public slots:
    void slideShowEffectFinished( const HbEffect::EffectStatus &status );
    void transitionEffectFinished( const HbEffect::EffectStatus &status );

private:
    void effectPluginResolver();
    void initTransitionEffect();
    void cleanTransitionEfffect();
    
private:
    GlxSlideShowSetting mSlideShowSetting;	
    int mNbrEffectRunning;               //containe the number of effect running
    GlxEffectPluginBase *mEffectPlugin;
    QHash<GlxEffect, GlxTransitionEffectSetting *> mTransitionEffectList;
    GlxEffect mTransitionEffect;
};

#endif /*GLXEFFECTENGINE_H*/
