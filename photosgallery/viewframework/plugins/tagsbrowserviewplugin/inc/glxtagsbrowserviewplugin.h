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
* Description:    Tag Browser view plugin definition.
 *
*/




#ifndef C_GLXTAGSBROWSERVIEWPLUGIN_H
  #define C_GLXTAGSBROWSERVIEWPLUGIN_H

  // INCLUDES
  #include <mpxaknviewplugin.h>
  #include <glxmedialistfactory.h>

  // CLASS DECLARATION

  /**
   * Gallery tag browser view plugin. It uses the Cloud view of Gallery.
   *
   *  @lib glxtagsbrowserviewplugin.lib
   *  @since S60 v3.2
   */
  NONSHARABLE_CLASS(CGlxTagsBrowserViewPlugin): public CMPXAknViewPlugin,
    public MGlxMediaListFactory
  {
  public:

    /**
     * Two-phased constructor.
     *
     * @since 3.2
     * @return Pointer to newly created object.
     */
    static CGlxTagsBrowserViewPlugin *NewL();

    /**
     * Destructor.
     */
    virtual ~CGlxTagsBrowserViewPlugin();

    /* Creates and returns a Media List. Ownership of Media List is transfered.
	 * @param aCollectionUtility The collection utility to use when creating the media list
	 * @return The Media List (ownership transfered to caller)
	 */
	 MGlxMediaList& CreateMediaListL(MMPXCollectionUtility& aCollectionUtility) const;

 
  private:

    /**
     * From CMPXAknViewPlugin
     * Construct Avkon view.
     *
     * @since S60 v3.2
     * @return Pointer to a newly created Avkon view.
     */
    CAknView *ConstructViewLC();

  private:

    /**
     * C++ default constructor.
     */
    CGlxTagsBrowserViewPlugin();

    /**
     *Symbian 2nd phase Constructor 
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
  };

#endif // C_GLXTAGSBROWSERVIEWPLUGIN_H

// End of File
