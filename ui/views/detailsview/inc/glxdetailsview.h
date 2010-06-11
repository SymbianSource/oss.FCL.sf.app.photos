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
 
#ifndef GLXDETAILSVIEW_H
#define GLXDETAILSVIEW_H

#include <glxview.h>


class GlxMediaModel;
class QAbstractItemModel;
class HbIconItem;

class GlxFavMediaModel;
class HbDocumentLoader;
class HbLabel;
class HbPushButton;
class GlxDetailsNameLabel;
class GlxDetailsDescriptionEdit;
class GlxDetailsViewDocLoader;

class GlxDetailsView : public GlxView  
{

    Q_OBJECT

public:
    /*
     * class constructor.
     */
    GlxDetailsView(HbMainWindow *window);
    /*
     * class Destructor.
     */
    ~GlxDetailsView();

    /*
     * This is called from the view manager when the view is going to be activated.
     */
    void activate() ;

    /*
     * This is called from the view manager when the view is going to be de-activated.
     */
    void deActivate();

    /*
     * This is called from the view manager before the view is going to Activated.
     */
    void initializeView(QAbstractItemModel *model);

    /*
     * This is called from the view manager before the view is going to de-activated.
     */
    void resetView();

    /*
     * Sets the model corresponding to this view.
     */
    void setModel (QAbstractItemModel *model);
    /*
     * return the view item for animation
     */    
    QGraphicsItem * getAnimationItem(GlxEffect transtionEffect);


public slots:
    void updateLayout(Qt::Orientation);
    void rowsRemoved(const QModelIndex &parent, int start, int end);
    void dataChanged(QModelIndex startIndex, QModelIndex endIndex);
    void updateFavourites();
    void UpdateDescription();
    void FillDetails();
    
private:

    /*
     * Retrieves the image from the model and shows the image
     */
    void showImage();

    /*
     * Sets the Date to the label recieved from MDS
     */
    void setImageName();
    
    void setDesc();

    /*
     * Sets the Date to the label received from MDS
     */
    void setDate();
    
    /*
     * Sets the time to the label received from MDS
     */
    void setTime();
    
    /*
     * Sets the size to the label received from MDS
     */
    void setSize();    
    
    /*
     * create the favourite model
     */
    void setFavModel();
    
    /*
     * Add all the model releted connection and data
     */
    void initializeNewModel();
    /*
     * clear all the model releted connection and data
     */
    void clearCurrentModel();
    
    /*
     * cleans up the resources used.
     */
    void cleanUp();
    
    QString sizeinStrings(int size);

    /*
     * connects the items to slot to respective signals
     */
    void setConnections();

    /*
     * clear the connections
     */
    void clearConnections();
    
private:
    
    //Contains the thumbnail shown in teh details view.
    HbLabel *mDetailsIcon;
    
    //Contains the favourite icon which adds or removes the image to favourite folder
    HbPushButton *mFavIcon;
   
    //The media Model to acess the attributes; not owned,dont delete.
    QAbstractItemModel *mModel; 
    
    //The media Model to acess the attributes whether image is in favourite folder or not;owned by this view
    GlxFavMediaModel *mFavModel;    
 
    //The reference to the Main Window
    HbMainWindow *mWindow;
     
    int mSelIndex;
    
   //Document Loader  
    GlxDetailsViewDocLoader *mDocLoader;
  
    //Shows the Images
    GlxDetailsNameLabel *mImageName;
	
	//Shows the descriptions
     GlxDetailsDescriptionEdit *mDescriptions;
	
	//Shows the Date 
    HbLabel *mDateLabel;
	
	//Shows the size of the image
    HbLabel *mSizeLabel;
	
	//Shows the time 
    HbLabel *mTimeLabel;    
};
#endif //GLXDETAILSVIEW_H
