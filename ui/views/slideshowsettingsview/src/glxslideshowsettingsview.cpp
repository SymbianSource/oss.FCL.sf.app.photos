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



//Includes
#include <hbcombobox.h>
#include <hbmainwindow.h>
#include <hblabel.h>
#include <QSize>

//User Includes
#include "glxuistd.h"
#include "glxviewids.h"
#include "glxslideshowsettingsview.h"
#include "glxsettinginterface.h"
#include "glxlocalisationstrings.h"


GlxSlideShowSettingsView::GlxSlideShowSettingsView(HbMainWindow *window) 
    : GlxView ( GLX_SLIDESHOWSETTINGSVIEW_ID ), 
      mEffect(NULL),
      mDelay(NULL),
      mWindow(window), 
      mContextlabel (NULL),
      mEffectlabel (NULL),
      mDelaylabel (NULL),
      mSettings( NULL )
{
    mSettings = GlxSettingInterface::instance() ;
  	setContentFullScreen( true );
}

GlxSlideShowSettingsView::~GlxSlideShowSettingsView()
{
    delete mContextlabel;
    delete mEffectlabel;
    delete mDelaylabel;
    delete mEffect;
    delete mDelay;
}

void GlxSlideShowSettingsView::setModel(QAbstractItemModel *model)
{
    Q_UNUSED( model )
    return;
}

void GlxSlideShowSettingsView::orientationChanged(Qt::Orientation)
{
    setLayout();
}

void GlxSlideShowSettingsView::activate()
{
    connect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationChanged(Qt::Orientation)));
    
    if ( mContextlabel == NULL ) {
        mContextlabel = new HbLabel( GLX_MENU_SLIDESHOW, this );
    }
    
    if ( mEffectlabel == NULL ) {
        mEffectlabel = new HbLabel( GLX_LABEL_TRANSITION_EFFECT, this );
    }
    
    if ( mEffect == NULL ) {
        mEffect = new HbComboBox(this);
        QStringList effectList = mSettings->slideShowEffectList();
        mEffect->addItems( effectList );        
    }
    
    if ( mDelaylabel == NULL ) {
        mDelaylabel = new HbLabel( GLX_LABEL_TRANSITION_DELAY, this );
    }
    
    if ( mDelay == NULL ) {
        mDelay = new HbComboBox(this);
        QStringList delayList;
        delayList <<  GLX_VAL_SLOW << GLX_VAL_MEDIUM << GLX_VAL_FAST ;
        mDelay->addItems( delayList );
    }
   
   // Read the values from the cenrep    
    mEffect->setCurrentIndex( mSettings->slideShowEffectIndex() );
    mDelay->setCurrentIndex( mSettings->slideShowDelayIndex() );
    setLayout();
}

void GlxSlideShowSettingsView::setLayout()
{
    QSize sz = screenSize();
    
    mContextlabel->setGeometry( 5, 25, sz.width() - 50, 100);
    mEffectlabel->setGeometry(5, 75, sz.width() - 50, 100 );
    mEffect->setGeometry(15, 150, sz.width() - 50, 100 );
    mDelaylabel->setGeometry(5, 180, sz.width() - 50, 100 );
    mDelay->setGeometry(15, 255, sz.width() - 50, 100 );
}

void GlxSlideShowSettingsView::deActivate()
{
	 //Store the current effect and delay before going back to the previous view
	mSettings->setslideShowEffectIndex( mEffect->currentIndex() );
	mSettings->setSlideShowDelayIndex( mDelay->currentIndex() ); 
    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationChanged(Qt::Orientation)));
}

