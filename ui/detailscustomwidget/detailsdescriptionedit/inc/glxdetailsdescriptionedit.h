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
*  Description:   This is custom widget for details view in photos for displaying the description of image .
*
*/
 
#ifndef GlXDETAILSDESCEDIT_H
#define GlXDETAILSDESCEDIT_H

#include <hbtextedit.h>

#ifdef BUILD_DESCEDIT
#define MY_EXPORT Q_DECL_EXPORT
#else
#define MY_EXPORT Q_DECL_IMPORT
#endif

class HbEditorInterface;
class MY_EXPORT GlxDetailsDescriptionEdit : public HbTextEdit 
{	
    Q_OBJECT
    
public:
    GlxDetailsDescriptionEdit(QGraphicsItem *parent = NULL);
    ~GlxDetailsDescriptionEdit();    	
    void setItemText( const QString &text );
		
protected:
    void  focusInEvent (QFocusEvent *event) ;
    void  focusOutEvent (QFocusEvent *event) ;

    
signals :
     void labelPressed();
private:
     HbEditorInterface *mDesc;     
};

#endif // GlXDETAILSDESCEDIT_H
