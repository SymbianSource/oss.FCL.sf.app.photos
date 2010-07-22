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



#ifndef GLXSLIDESHOWVIEW_H
#define GLXSLIDESHOWVIEW_H

//User Includes
#include "glxview.h"
#include "glxslideshowwidget.h"

//Qt/Orbit forward declarations
class HbMainWindow;
class HbDocumentLoader;

//User Includes forward Declaration
class GlxView;
class GlxSlideShowWidget;
class GlxTvOutWrapper;

class GlxSlideShowView : public GlxView
{
    Q_OBJECT

public:
    GlxSlideShowView(HbMainWindow *window,HbDocumentLoader *Docloader);
    ~GlxSlideShowView();
    void activate() ;
    void deActivate();
    void setModel(QAbstractItemModel *model);
    void setModelContext();
        
public slots :
    void orientationChanged(Qt::Orientation);   
    void slideShowEventHandler( GlxSlideShowEvent e);
    void indexchanged();
    void modelDestroyed();
    
protected :
    bool eventFilter(QObject *obj, QEvent *ev);
    
private:
    void loadObjects();
private:
    QAbstractItemModel  *mModel;
    HbMainWindow        *mWindow;
    GlxSlideShowWidget  *mSlideShowWidget;    
    HbDocumentLoader    *mDocLoader; //Docml loader to load the widgets from docml
    GlxTvOutWrapper*    mTvOutWrapper;
};

#endif /*GLXSLIDESHOWVIEW_H*/
