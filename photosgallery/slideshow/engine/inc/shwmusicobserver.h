/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Music control observer interface 
*
*/




#ifndef __MSHWMUSICOBSERVER_H__
#define __MSHWMUSICOBSERVER_H__

#include <e32std.h>

/*! 
  @class MShwMusicObserver
  
  @discussion Observes for changes in the music controls volume and on/off state
              MPX-specific
  */
class MShwMusicObserver
    {
    public:

        /**
         * Notification that music is playing.
         * If this method leaves, MusicOff will
         * be called.
         */
        virtual void MusicOnL() = 0;

        /**
         * Notification that music is not playing
         */
        virtual void MusicOff() = 0;

        /**
         * Notification that music volume has changed
         * @param aCurrentVolume The current volume 
         * @param aCurrentVolume The maximum volume 
         */
        virtual void MusicVolumeL( TInt aCurrentVolume, TInt aMaxVolume ) = 0;

        /**
         * Notification that music track had an error
         * @param aErrorCode The error code
         */
        virtual void ErrorWithTrackL( TInt aErrorCode ) = 0;

    protected:

        /** 
         * Destructor. No deletion through this interface.
         */
        virtual ~MShwMusicObserver() {};

    };

#endif // __MSHWMUSICOBSERVER_H__

