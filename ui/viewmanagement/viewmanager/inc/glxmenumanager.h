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
#include <QObject>
#include <QPointF>
class HbMenu;
class GlxMenuManager : public QObject
{
Q_OBJECT

public :
	GlxMenuManager();
	~GlxMenuManager();
    void CreateViewMenu(qint32 viewId,HbMenu* menu,bool empty = false , int subState = -1);
    void createMarkingModeMenu(HbMenu* menu);
    void ShowItemSpecificMenu(qint32 viewId,QPointF pos);
signals :
    void commandTriggered(qint32 commandId);
public slots:

protected:
	
private slots:
    void menuItemSelected();
    
private:
    void CreateGridMenu(HbMenu* menu);
    void CreateListMenu(HbMenu* menu);
    void CreateFullscreenMenu(HbMenu* menu);
    void CreateImageViewerMenu(HbMenu* menu);
private:

};
