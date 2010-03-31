/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Transition Effects Header
*
*/

#ifndef GLXGFXTRANSEFFECT_H_
#define GLXGFXTRANSEFFECT_H_

#include<glxgallery.hrh>

//Photos uid
const TUid KPhotosUid = TUid::Uid(KGlxGalleryApplicationUid);

//Transition id for view activation for grid, list and cloud views
const TUint KActivateTransitionId (1000);

//Transition id for view De-activation for grid, list and cloud views
const TUint KDeActivateTransitionId (1001);

//Transition id for view De-activation for fullscreen view
const TUint KFSDeActivateTransitionId (1003);

#endif /* GLXGFXTRANSEFFECT_H_ */
