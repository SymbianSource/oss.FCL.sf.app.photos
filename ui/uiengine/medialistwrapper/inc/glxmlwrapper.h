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

/*glxmlwrapper.h*/

#ifndef GLXMLWRAPPER_H
#define GLXMLWRAPPER_H

#include <QObject>
#include <glxfiltergeneraldefs.h>
#include <QtGlobal>
#include <glxuistd.h>
#include <QSize>
#include <qdatetime.h>
//forward declarations
class GlxMLWrapperPrivate;
class HbIcon;
//constant declarations


#ifdef BUILD_MLWRAPPER
#define GLX_MLWRAPPER_EXPORT Q_DECL_EXPORT
#else
#define GLX_MLWRAPPER_EXPORT Q_DECL_IMPORT
#endif


class GLX_MLWRAPPER_EXPORT GlxMLWrapper : public QObject
{
    Q_OBJECT
public:
	/**
	*constructor
	*/
    GlxMLWrapper(int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType);
	/**
	*destructor
	**/
	~GlxMLWrapper();
    /**
     * for setting the attribute context mode will be used mainly for retreiving thumbnail of different sizes.
     */
    void setContextMode(GlxContextMode contextMode);
     /**
     * for retreiving the data Count.
     */
    int getItemCount();
	/**
	* retrieveItemIcon()
	*/
	HbIcon* retrieveItemIcon(int index, GlxTBContextType aTBContextType);
    void itemsAdded(int startIndex,int endIndex);
    void itemsRemoved(int startIndex,int endIndex);
    void handleReceivedIcon(int itemIndex, GlxTBContextType tbContextType);
    void handleIconCorrupt(int itemIndex);
    void handleGeneralError(int error);
	void handleListItemAvailable(int itemIndex);
    int getFocusIndex() const;
    void setFocusIndex(int itemIndex);
    
    void setSelectedIndex(int aItemIndex);

	QString retrieveListTitle(int index);
	QString retrieveListSubTitle(int index);
    int getVisibleWindowIndex();
    void setVisibleWindowIndex(int itemIndex);
	QString retrieveItemUri(int index);
	QSize retrieveItemDimension(int index);
	QDate retrieveItemDate(int index);
	int retrieveItemFrameCount(int index);

signals:
     void updateItem(int index, GlxTBContextType tbContextType);
	 void insertItems(int startIndex,int endIndex);
	 void removeItems(int startIndex,int endIndex);
	 void itemCorrupted(int itemIndex);

private:
	GlxMLWrapperPrivate* mMLWrapperPrivate;

};

#endif //GLXMLWRAPPER_H
