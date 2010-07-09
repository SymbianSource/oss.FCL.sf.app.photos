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



#ifndef GLXMEDIAMODEL_H
#define GLXMEDIAMODEL_H

#include <QAbstractItemModel>
#include <glxuistd.h>
#include <QDateTime>
#include <glxexternalinterfacedefs.h>
//forward declarations

#ifdef BUILD_MEDIAMODEL
#define GLX_MEDIAMODEL_EXPORT Q_DECL_EXPORT
#else
#define GLX_MEDIAMODEL_EXPORT Q_DECL_IMPORT
#endif

class HbIcon;
class GlxModelParm;
class GlxMLWrapper;
class GlxDRMUtilityWrapper;
#include <QCache>
#include <QVector>

class GLX_MEDIAMODEL_EXPORT GlxMediaModel : public QAbstractItemModel
{
Q_OBJECT
public :	
	
    GlxMediaModel(GlxModelParm & modelParm);
    ~GlxMediaModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

	/*
	*for setting external image data to the model. 
	*/
	void addExternalItems(GlxExternalData* externalItems);
	/*
	*for removing all external image data from the model. 
	*/
	void clearExternalItems();
	/*
	* for checking if the item is editab
	*/

signals :
	void albumTitleAvailable(QString aTitle);
	void populated();

private:
	HbIcon* GetGridIconItem(int itemIndex, GlxTBContextType tbContextType) const;
	HbIcon* GetFsIconItem(int itemIndex,GlxTBContextType tbContextType)const;
	HbIcon* GetExternalIconItem(int itemIndex,GlxTBContextType tbContextType)const;
/**
 * for setting the attribute context mode will be used mainly for retreiving thumbnail of different sizes.
 */
    void setContextMode(GlxContextMode contextMode);
	/*
	 *Removes the context
	 */
    void removeContextMode(GlxContextMode contextMode);
    void setFocusIndex(const QModelIndex &index);
    QModelIndex getFocusIndex() const;
    void setSelectedIndex(const QModelIndex &index);
    HbIcon *getCorruptDefaultIcon ( const QModelIndex &index ) const;

	
signals :
    void iconAvailable(int itemIndex, HbIcon* itemIcon, GlxTBContextType tbContextType) const;
	/*
	 *signal to update the details view
	 */
    void updateDetailsView();
public slots:
	void itemUpdated1(int mlIndex, GlxTBContextType tbContextType);
	void itemsAdded(int startIndex, int endIndex);
	void itemsRemoved(int startIndex, int endIndex);
	void itemCorrupted(int itemIndex);
	void albumTitleUpdated(QString aTitle);
	void modelpopulated();
	/*
	 *Emits signal to update the details view
	 */
	void updateDetailItems();
protected:
	
private slots:
    void updateItemIcon(int itemIndex, HbIcon* itemIcon, GlxTBContextType tbContextType);
    
private:
	GlxMLWrapper* mMLWrapper;	
	QCache<int, HbIcon> itemIconCache;
	QCache<int, HbIcon> itemFsIconCache;
	HbIcon* m_DefaultIcon;
	HbIcon* m_CorruptIcon;
	
	GlxContextMode mContextMode;
	//for external data to be populated by model
	GlxExternalData* mExternalItems;
	QCache<int, HbIcon> itemExternalIconCache;
	int externalDataCount;
	int mFocusIndex;
	int mSubState;
	GlxDRMUtilityWrapper* mDRMUtilityWrapper;
	int mTempVisibleWindowIndex;
};


#endif /* GLXMEDIAMODEL_H_ */
