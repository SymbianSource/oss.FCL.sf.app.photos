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



#ifndef GLXMODELPARM_H
#define GLXMODELPARM_H

#include<glxfiltergeneraldefs.h>
#include<glxuistd.h>

enum
{
    GlxHgVisibilityRole = Qt::UserRole + 1, // VisibilityRole For MediaWall ,  Dont Change This Value - Depedency To HgWidget
    GlxFsImageRole,                         //to get the fullscreen image
    GlxContextRole,                     //to get and set the context   
    GlxFocusIndexRole,                  //to get and set the selected item index
    GlxVisualWindowIndex,               //to set the visual window index in media list
    GlxUriRole,                         //to get the URI of the image
    GlxDimensionsRole,                  
    GlxSelectedIndexRole,               //to get and set the seleceted item list
    GlxDateRole,                        //to get the image created or modified date 
    GlxFrameCount,                      //to get the number of frame in an image
    GlxSubStateRole,                    //to publish the sub state of a SM states
    GlxHdmiBitmap,                      //to get the pre decoded thumbnail for HDMI
	GlxListItemCount,                   //to get the number of item in the album list
	GlxSystemItemRole,                   //to get the item is system item or not
    GlxFavorites,                       //to get the image is in favorites folder or not
    GlxQImageSmall,                     //To Get Grid Thumbnail as QImage
    GlxQImageLarge,                     //To Get FullScreen Thumbnail as QImage
    GlxDefaultImage,					//To Get Default Thumbnail 
    GlxViewTitle,
    GlxPopulated
};


class GlxModelParm
{
public:
	GlxModelParm (qint32 collectionId = 0, qint32 depth = 0 , TGlxFilterItemType filterType = EGlxFilterItemAll , GlxContextMode contextMode = GlxContextPtGrid ,QString uri = NULL) 
	    : mCollectionId(collectionId) ,
	      mDepth(depth) , 
	      mFilterType( filterType ),
	      mContextMode( contextMode ),
          mUri(uri)
	{
	
	}
	
	void setCollection (qint32 collectionId = 0) { mCollectionId = collectionId ; }
	qint32 collection() { return mCollectionId; }
	void setDepth (qint32 depth = 0) { mDepth = depth ; }
	qint32 depth() { return mDepth; }
	QString path() { return mUri; }
	void setPath(QString uri = NULL) {  mUri=uri; }
	TGlxFilterItemType filterType() { return mFilterType; }
	void setContextMode( GlxContextMode contextMode ) { mContextMode = contextMode ; }
	GlxContextMode contextMode() { return mContextMode ; }
	
private:
	qint32 mCollectionId;
	qint32 mDepth;
	TGlxFilterItemType mFilterType;
	GlxContextMode mContextMode;
	QString mUri;
};


#endif /* GLXMODELPARM_H_ */
