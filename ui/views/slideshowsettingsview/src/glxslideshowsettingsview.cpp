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
#include "glxmodelparm.h"




GlxSlideShowSettingsView::GlxSlideShowSettingsView(HbMainWindow *window) 
    : GlxView ( GLX_SLIDESHOWSETTINGSVIEW_ID ), 
      mDelay(NULL),
      mEffect(NULL), 
      mContextlabel (NULL),
      mEffectlabel (NULL),
      mDelaylabel (NULL),
      mWindow(window)
{
	 	mTempEffect = 0;
  	mTempDelay =0 ;
  	setContentFullScreen( true );
}

GlxSlideShowSettingsView::~GlxSlideShowSettingsView()
{
		if(mContextlabel) {
    	delete mContextlabel;
    	mContextlabel = NULL;
  	}
    if(mEffectlabel) {
    	delete mEffectlabel;
    	mEffectlabel = NULL;
    }
    if(mDelaylabel) {
    	delete mDelaylabel;
    	 mDelaylabel = NULL;
    }
    if(mEffect) {
    	delete mEffect;
    	mEffect = NULL;
    }
    if(mDelay) {
    	delete mDelay;
    	mDelay = NULL;
    }
}

void GlxSlideShowSettingsView::setModel(QAbstractItemModel *model)
{
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
        mContextlabel = new HbLabel("Slideshow", this);
    }
    
    if ( mEffectlabel == NULL ) {
        mEffectlabel = new HbLabel("Transition effect:", this);
    }
    
    if ( mEffect == NULL ) {
        mEffect = new HbComboBox(this);
        QStringList effectList;
        effectList<<"wave"<<"smooth fade"<<"zoom to face";
        mEffect->addItems( effectList );
        
    }
    
    if ( mDelaylabel == NULL ) {
        mDelaylabel = new HbLabel("Transition delay:", this);
    }
    
    if ( mDelay == NULL ) {
        mDelay = new HbComboBox(this);
        QStringList delayList;
        delayList<<"slow"<<"medium"<<"fast";
        mDelay->addItems( delayList );
    }
   
    mEffect->setCurrentIndex( mTempEffect);
    mDelay->setCurrentIndex ( mTempDelay );
    
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
    mTempEffect = mEffect->currentIndex();
    mTempDelay = mDelay->currentIndex();
    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationChanged(Qt::Orientation)));
}



