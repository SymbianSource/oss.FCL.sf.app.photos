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
/*
 * glximgvwrmetadatacontainer.cpp
 *
 *  Created on: Oct 22, 2009
 *      Author: sourbasu
 */


#include  "glximgvwrmetadatacontainer.h"
#include  "glxustringconverter.h"                // converts the symbian types to UString type
#include  <AknUtils.h>
#include  <StringLoader.h>
#include  <glxmetadatadialog.rsg>
#include  <glxviewbase.rsg>
#include  <glxlog.h>
#include  <glxtracer.h>
#include  <glxscreenfurniture.h>
#include  <glxdetailsmulmodelprovider.h>          //Details data provider
#include  <glxdetailsboundcommand.hrh>
#include  <glxcommandhandleraddtocontainer.h>     // For CGlxCommandHandlerAddToContainer
#include  <glxcommandhandlers.hrh>                // for command handler id
#include  <glxcommandfactory.h>                   //for command factory
#include  <mpxcommandgeneraldefs.h>               // Content ID identifying general category of content provided

#include  <glxtextentrypopup.h>    
#include  <glxcollectionpluginall.hrh>
#include  <glxuistd.h>
#include  <glxcollectionplugintags.hrh>           // tag collection plugin uid
#include  <glxthumbnailattributeinfo.h>           // KGlxMediaIdThumbnail
#include  <glxattributeretriever.h>               // CGlxAttributeReteiver
#include  <aknQueryControl.h>
#include  <glxdrmutility.h>                       //For launching DRM details pane
#include  <glxgeneraluiutilities.h>               // General utilties class definition
#include  <ExifModify.h>    
#include  <glxuiutilities.rsg>                    //For CExifModify
#include  <mpxmediadrmdefs.h>
#include  <glxfilterfactory.h>
#include <glxcollectionpluginimageviewer.hrh>

//marquee

#include <eikfrlb.h>
#include <eikfrlbd.h>
const TInt KMaxMediaPopupTitleLength = 0x100;
const TInt KMediaListId              = 0x2000D248;
const TInt KMarqueeLoopCount         = 3;
const TInt KMarqueeScrollAmount      = 20;
const TInt KMarqueeScrollDelay       = 1000000;
const TInt KMarqueeScrollInterval    = 200000;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// NewL
// ---------------------------------------------------------
//  
CGlxImgVwrMetadataContainer* CGlxImgVwrMetadataContainer::NewL( const TRect& aRect,const TDesC& item)
    {
    TRACER("CGlxImgVwrMetadataContainer::NewL");  
    CGlxImgVwrMetadataContainer* self = CGlxImgVwrMetadataContainer::NewLC( aRect,item);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// NewLC
// ---------------------------------------------------------
//  
CGlxImgVwrMetadataContainer* CGlxImgVwrMetadataContainer::NewLC( const TRect& aRect,const TDesC& aUri)
    {
    TRACER("CGlxImgVwrMetadataContainer::NewLC"); 
    CGlxImgVwrMetadataContainer* self = new(ELeave) CGlxImgVwrMetadataContainer(aUri);
    CleanupStack::PushL(self);
    self->ConstructL( aRect);
    return self;
    }

// ---------------------------------------------------------
// CGlxImgVwrMetadataContainer
// ---------------------------------------------------------
//  
CGlxImgVwrMetadataContainer::CGlxImgVwrMetadataContainer(const TDesC& aUri)
:iUri(aUri)
        {
        // No implementation
        }

// ---------------------------------------------------------
// CGlxImgVwrMetadataContainer::ConstructL
// ---------------------------------------------------------
//  
void CGlxImgVwrMetadataContainer::ConstructL( const TRect& /*aRect*/ )
    {   
    TRACER("CGlxMetadataContainer::ConstructLL()");
    //Creating the RBuf texts for all the items except tags & albums
    //which would be updated as whne the item is edited
    iTextSetter.CreateL(KMaxFileName);
    CreateMediaListForSelectedItemL();
    }

// ---------------------------------------------------------
// ~CGlxImgVwrMetadataContainer
// ---------------------------------------------------------
//  
CGlxImgVwrMetadataContainer::~CGlxImgVwrMetadataContainer()
    {
    TRACER("CGlxImgVwrMetadataContainer::~CGlxImgVwrMetadataContainer");
    if( iItemMediaList ) 
        {
        iItemMediaList->RemoveContext(iMainListAttributecontext);
        iItemMediaList->RemoveMediaListObserver(this);
        iItemMediaList->Close();
        iItemMediaList = NULL;  
        } 
    if( iMainListAttributecontext )
        {
        delete iMainListAttributecontext;
        iMainListAttributecontext = NULL;
        }
    if( IsVisible() )
        {
        MakeVisible(EFalse);
        }
    iTextSetter.Close();
    }

//-----------------------------------------------------------------------------
// CGlxImgVwrMetadataContainer::MediaList
//-----------------------------------------------------------------------------
MGlxMediaList& CGlxImgVwrMetadataContainer::MediaList()
    {
    TRACER("CGlxMetadataContainer::HandleAttributesAvailableL()");
    //returns the active medialist.
    return *iItemMediaList;    
    }

//-----------------------------------------------------------------------------
// CGlxImgVwrMetadataContainer::CreateSettingItemL
//-----------------------------------------------------------------------------
CAknSettingItem* CGlxImgVwrMetadataContainer::CreateSettingItemL(TInt aResourceId)
    {
    TRACER("CGlxImgVwrMetadataContainer::CreateSettingItemL");      
    CAknSettingItem* settingItem = NULL; // No need to push onto cleanup stack
    iTextSetter.Zero();  

    //Creating a empty Settings list box which will  be populated with metadata in handleattributeavailable
    switch(aResourceId)
        {
        case EImgVwrNameItem:
        case EImgVwrDateAndTimeItem:
        case EImgVwrDescriptionItem:
            {
            settingItem = new (ELeave) CAknTextSettingItem( 
                    aResourceId, iTextSetter );

            break;          
            }
        case EImgVwrSizeItem:            
        case EImgVwrResolutionItem:
            {
            settingItem = new (ELeave) CAknTextSettingItem( 
                    aResourceId, iTextSetter );

            break;          
            }            

        case EImgVwrlicenseItem:
            {
            settingItem = new (ELeave) CAknTextSettingItem( 
                    aResourceId, iTextSetter );
            //Hide the item until we get the attributes
            //where in we check for the usage rights.                                                                                                            
            settingItem->SetHidden(ETrue);
            //Required to refresh the listbox when any items visiblity is changed
            this->HandleChangeInItemArrayOrVisibilityL();
            }
            break;

        default:
            {
            break;    
            }
        }
    return settingItem;
    }
//-----------------------------------------------------------------------------
// CGlxImgVwrMetadataContainer::IsItemModifiable
//-----------------------------------------------------------------------------
TBool CGlxImgVwrMetadataContainer::IsItemModifiable()
    {
    TRACER("CGlxMetadataContainer::IsItemModifiable()");
    //Only items like name , description, tag and albums are modifiable
    //The check is for the items from ENameItem(0) tille ETagsItem(4)
    if(ListBox()->CurrentItemIndex()<=ETagsItem)
        {
        return EFalse;
        }
    //return ETrue to dim the item
    return ETrue;

    }

//-----------------------------------------------------------------------------
// CGlxImgVwrMetadataContainer::HandleListBoxEventL
//-----------------------------------------------------------------------------
void CGlxImgVwrMetadataContainer::HandleListBoxEventL(CEikListBox*  /*aListBox*/,
        TListBoxEvent aEventType)
    {
    TRACER("CGlxMetadataContainer::HandleListBoxEventL()");
    GLX_LOG_INFO("CGlxImgVwrMetadataContainer::HandleListBoxEventL");         
    if (aEventType == EEventItemSingleClicked)
        {
        if(iItemMediaList->Count() == 0)
            {
            GLX_LOG_INFO("CGlxImgVwrMetadataContainer:: NO Items");         
            return;
            }
        TInt index = ListBox()->CurrentItemIndex() ;
        if(EImgVwrlicenseItem == index)
            {
            GLX_LOG_INFO("CGlxImgVwrMetadataContainer::Licence item");         
            CGlxDRMUtility* drmUtility = CGlxDRMUtility::InstanceL();
            CleanupClosePushL(*drmUtility);
            drmUtility->ShowDRMDetailsPaneL(iItemMediaList->Item(0).Uri());
            CleanupStack::PopAndDestroy(drmUtility);
            }
        }
    }

// ----------------------------------------------------------------------------
// CGlxImgVwrMetadataContainer::ViewDynInitMenuPaneL
// ----------------------------------------------------------------------------
// 
void CGlxImgVwrMetadataContainer::ViewDynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane)
    {
    TRACER("CGlxMetadataContainer::ViewDynInitMenuPaneL()");
    if( aMenuId == R_METADATA_MENU )
            {
            //Set dim the options based on the utem selected
            //Viewdetails option will be availble only for the license item
            aMenuPane->SetItemDimmed(KGlxViewBoundMenuCommandId,IsLicenseItem());
            }
    }

//Medialist callbacks.    
// ----------------------------------------------------------------------------
// CGlxImgVwrMetadataContainer::HandleAttributesAvailableL
// ----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataContainer::HandleAttributesAvailableL( TInt /*aItemIndex*/, 
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList )
    {
    TRACER("CGlxMetadataContainer::HandleAttributesAvailableL()");
    //generic medialist for the item for all the attributes required other than tags and albums.
    TInt x = aAttributes.Count();
    if(aList == iItemMediaList)
        {
        // Loop untill it checks for all the avialable attributes
        for (TInt i = aAttributes.Count() - 1; i >= 0 ; i--)
            {
            //set attributes to the items in the container
            SetAttributesL(aAttributes[i]);                   
            }
        }

    }

// ----------------------------------------------------------------------------
// HandleItemAddedL
// ----------------------------------------------------------------------------
// 
void CGlxImgVwrMetadataContainer::HandleItemAddedL( TInt /*aStartIndex*/, TInt /*aEndIndex*/, 
        MGlxMediaList* aList )
    {
    TRACER("CGlxMetadataContainer::HandleItemAddedL()");
    if(!iMarquee)
        {
            EnableMarqueingL();
        }
    SetDurationLIicenseItemVisibilityL();
    if(aList == iItemMediaList)
        {
        if(iItemMediaList->Count())
            {
            TGlxMedia item = iItemMediaList->Item(0);
            CGlxUStringConverter* stringConverter = CGlxUStringConverter::NewL();
            CleanupStack::PushL(stringConverter );
            for(TInt index = 0; index <= EImgVwrlicenseItem; index++)
                {
                HBufC* string = NULL;               
				iTextSetter.Zero();

                if(index == EImgVwrSizeItem)
                    {
                    stringConverter->AsStringL(item,
                            KMPXMediaGeneralSize,0, string );              
                    }
                else if(index == EImgVwrNameItem)
                    {
                    stringConverter->AsStringL(item,
                            KMPXMediaGeneralTitle,0, string );
                    }
                else if(index == EImgVwrDateAndTimeItem)
                    {  
                    stringConverter->AsStringL( item, 
                            KGlxMediaGeneralLastModifiedDate,
                            R_QTN_DATE_USUAL_WITH_ZERO,string );
                    }
                else if(index == EImgVwrDescriptionItem)
                    {
                    stringConverter->AsStringL(item,
                            KMPXMediaGeneralComment,0, string ); 
                    }
                else if(index == EImgVwrResolutionItem)
                    {
                    stringConverter->AsStringL(item,
                            KGlxMediaGeneralDimensions,0, string );
                    }    
                else if(index == EImgVwrlicenseItem)
					{
					// If an item is DRM protected, License field in details
					// should display "View Details"
					string = StringLoader::LoadL(R_GLX_METADATA_VIEW_OPTIONS_VIEW);
					}                
                else 
                    {
                    //no implementation
                    } 
                if(string)
                    {
                    iTextSetter.Zero();
                    iTextSetter.Append(*string);
                    }
                CleanupStack::PushL( string );
                EditItemL(index,EFalse);   
                CleanupStack::PopAndDestroy(string );           
                }
            CleanupStack::PopAndDestroy(stringConverter );
            }   
        }


    }
// ----------------------------------------------------------------------------
// EnableMarqueingL
// ----------------------------------------------------------------------------
//  
void CGlxImgVwrMetadataContainer::EnableMarqueingL()
    {
    TRACER("CGlxImgVwrMetadataContainer::EnableMarqueingL()");    
    iMarquee = ETrue;
    ListBox()->UseLogicalToVisualConversion(ETrue);
    ListBox()->ItemDrawer()->ColumnData()->SetMarqueeParams (KMarqueeLoopCount,
            KMarqueeScrollAmount, KMarqueeScrollDelay, KMarqueeScrollInterval);
    ListBox()->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
    }    
// ----------------------------------------------------------------------------
// HandleCommandCompleteL
// ----------------------------------------------------------------------------
// 
void CGlxImgVwrMetadataContainer::HandleCommandCompleteL(TAny* /*aSessionId*/, 
        CMPXCommand* /*aCommandResult*/, TInt /*aError*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleCommandCompleteL()");
    }

// ----------------------------------------------------------------------------
// HandleItemRemoved
// ----------------------------------------------------------------------------
//  
void CGlxImgVwrMetadataContainer::HandleItemRemovedL(  TInt /*aStartIndex*/, TInt /*aEndIndex*/, 
        MGlxMediaList* /*aList*/  )
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleItemRemovedL()");    
    }    
// ----------------------------------------------------------------------------
// HandleFocusChangedL
// ----------------------------------------------------------------------------
//  
void CGlxImgVwrMetadataContainer::HandleFocusChangedL( NGlxListDefs::
        TFocusChangeType /*aType*/, TInt /*aNewIndex*/, TInt /*aOldIndex*/, 
        MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleFocusChangedL()");
    }
// ----------------------------------------------------------------------------
// HandleItemSelected
// ----------------------------------------------------------------------------
//  
void CGlxImgVwrMetadataContainer::HandleItemSelectedL(TInt /*aIndex*/, 
        TBool /*aSelected*/, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleItemSelectedL");
    }
// ----------------------------------------------------------------------------
// HandleMessageL
// ----------------------------------------------------------------------------
//    
void CGlxImgVwrMetadataContainer::HandleMessageL( const CMPXMessage& /*aMessage*/, 
        MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleMessageL()");
    }
// ----------------------------------------------------------------------------
// HandleError
// ----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataContainer::HandleError( TInt /*aError*/ ) 
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleError()");
    TRAP_IGNORE(HandleErrorL());
    }

// ----------------------------------------------------------------------------
// HandleErrorL
// ----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataContainer::HandleErrorL()
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleErrorL()");
    }
// ----------------------------------------------------------------------------
// HandleCommandCompleteL
// ----------------------------------------------------------------------------
//  
void CGlxImgVwrMetadataContainer::HandleCommandCompleteL( CMPXCommand* /*aCommandResult*/, 
        TInt /*aError*/, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleCommandCompleteL()");
    }

// ----------------------------------------------------------------------------
// HandleMediaL
// ----------------------------------------------------------------------------
//  
void CGlxImgVwrMetadataContainer::HandleMediaL( TInt /*aListIndex*/, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleMediaL()");
    }

// ----------------------------------------------------------------------------
// HandleItemModifiedL
// ----------------------------------------------------------------------------
//  
void CGlxImgVwrMetadataContainer::HandleItemModifiedL( const RArray<TInt>& /*aItemIndexes*/,
        MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxImgVwrMetadataContainer::HandleItemModifiedL()");
    }
// ----------------------------------------------------------------------------
// ChangeMskL
// ----------------------------------------------------------------------------
//     
void CGlxImgVwrMetadataContainer::ChangeMskL()
    {
    TRACER("CGlxImgVwrMetadataContainer::ChangeMsk()");
    TInt index = ListBox()->CurrentItemIndex();
    CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();               
    switch(index)
        {
        case EImgVwrNameItem:        
        case EImgVwrDescriptionItem:
            {
            uiUtility->ScreenFurniture()->ModifySoftkeyIdL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                    EAknSoftkeyEdit,R_GLX_METADATA_MSK_EDIT);
            }        
            break;          
        case EImgVwrDateAndTimeItem:
        case EImgVwrSizeItem:
        case EImgVwrResolutionItem:
        case EImgVwrlicenseItem:
            {
            uiUtility->ScreenFurniture()->ModifySoftkeyIdL(CEikButtonGroupContainer::EMiddleSoftkeyPosition,
                    EAknSoftkeyEdit,R_GLX_METADATA_MSK_BLANK);            
            }        
            break;
        default:
            {
            break;    
            }
        }

    if ( uiUtility )
        {
        uiUtility->Close();
        }   
    }

//-----------------------------------------------------------------------------
// CGlxImgVwrMetadataContainer::CreateMediaListForSelectedItemL
//-----------------------------------------------------------------------------
void CGlxImgVwrMetadataContainer::CreateMediaListForSelectedItemL()
    {
    TRACER("CGlxMetadataContainer::CreateMediaListForSelectedItemL");

    //create the collection path for the medialist to be created
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    //set the all collection path as the details dialog can be launched from any of the grid views and filter with URI
    path->AppendL(/*KGlxCollectionPluginAllImplementationUid*/KGlxCollectionPluginImageViewerImplementationUid);
    //create the filter with the URI
    CMPXFilter* filter  = TGlxFilterFactory::CreateURIFilterL(iUri);  
    CleanupStack::PushL( filter );
    //create the medialist   
    iItemMediaList = MGlxMediaList::InstanceL(*path,TGlxHierarchyId(KMediaListId),filter);   

    //Add the attributes which are required to be displayed.
    iMainListAttributecontext = new (ELeave) CGlxAttributeContext(&iSelectionIterator);
    iMainListAttributecontext->AddAttributeL(KMPXMediaDrmProtected);
    iMainListAttributecontext->AddAttributeL(KMPXMediaGeneralSize);
    iMainListAttributecontext->AddAttributeL(KGlxMediaGeneralDimensions); 
    iMainListAttributecontext->AddAttributeL(KMPXMediaGeneralTitle);
    iMainListAttributecontext->AddAttributeL(KGlxMediaGeneralLastModifiedDate);
    iMainListAttributecontext->AddAttributeL(KMPXMediaGeneralComment); 

    //Add Context so that we get the handleattributes call once the medialist is populated with above mentioned attributes.
    iItemMediaList->AddContextL( iMainListAttributecontext,
            KGlxFetchContextPriorityBlocking );

    //add to observer for callbacks.
    iItemMediaList->AddMediaListObserverL(this);

    CleanupStack::PopAndDestroy( filter );
    CleanupStack::PopAndDestroy( path ); 
    }
// ----------------------------------------------------------------------------
// CGlxImgVwrMetadataContainer::SetAttributes
// ----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataContainer::SetAttributesL(TMPXAttribute attribute)
    {
    TRACER("CGlxImgVwrMetadataContainer::SetAttributesL");    

    if(!iSetVisible)
        {
        iSetVisible = ETrue;
        SetDurationLIicenseItemVisibilityL();
        }
    TGlxMedia item = iItemMediaList->Item(0);
    //Create the string convertor instance 
    //String convertor class with provide the specific format for date,location and duration and size.
    CGlxUStringConverter* stringConverter = CGlxUStringConverter::NewL();
    CleanupStack::PushL(stringConverter );
    HBufC* string = NULL;    

    //if attribute is date and time we need to pass the format it as R_QTN_DATE_USUAL_WITH_ZERO else null
    if(attribute == KGlxMediaGeneralLastModifiedDate)
        {
        stringConverter->AsStringL(item, 
                attribute,
                R_QTN_DATE_USUAL_WITH_ZERO, string );
        }    
    else
        {
        stringConverter->AsStringL(item,
                attribute,0, string );
        }
    //get the settings item based on the attribute and set the text.
    if ( string )
        {
        iTextSetter.Zero();
        iTextSetter.Append(*string);
        if(attribute == KMPXMediaGeneralSize)
            {
            EditItemL(EImgVwrSizeItem,EFalse);
            }    
        else if(attribute == KMPXMediaGeneralTitle)
            {
            EditItemL(EImgVwrNameItem,EFalse);
            }
        else if(attribute == KGlxMediaGeneralLastModifiedDate)
            {  
            EditItemL(EImgVwrDateAndTimeItem,EFalse);
            }
        else if(attribute == KMPXMediaGeneralComment)
            {
            EditItemL(EImgVwrDescriptionItem,EFalse);
            }
        else if(attribute == KGlxMediaGeneralDimensions)
            {
            EditItemL(EImgVwrResolutionItem,EFalse);
            }/*
        else if(attribute == KMPXMediaDrmProtected)
            {
            EditItemL(EImgVwrlicenseItem,EFalse);
            }*/
        else
            {
            
            } 
        delete string;
        string = NULL;
        }
    CleanupStack::PopAndDestroy(stringConverter );
    }

// ----------------------------------------------------------------------------
// CGlxMetadataContainer::EditItemL
// ----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataContainer::EditItemL(TInt aIndex, TBool /*aCalledFromMenu*/)
    {
    TRACER("CGlxImgVwrMetadataContainer::EditItemL");   
    CAknSettingItem* settingsitem = NULL;
    settingsitem = (*SettingItemArray())[aIndex];
    settingsitem->LoadL();
    settingsitem->UpdateListBoxTextL();
    settingsitem->StoreL();
    ListBox()->DrawNow();
    }



// ----------------------------------------------------------------------------
// CGlxMetadataContainer::SetAttributes
// ----------------------------------------------------------------------------
//
void CGlxImgVwrMetadataContainer::SetNameDescriptionL(TInt aItem)
    {
    TRACER("CGlxImgVwrMetadataContainer::SetNameDescriptionL");   
    //This function is commn for updatng both name and description once modified    
    //get the item handle to be modified       
    CAknSettingItem* settingsitem = (*SettingItemArray())[aItem]; 
    HBufC* textBuf = HBufC::NewLC( KMaxMediaPopupTitleLength );
    (textBuf->Des()).Copy((settingsitem->SettingTextL()));
    TPtr textPtr = textBuf->Des();
    TBuf<KMaxMediaPopupTitleLength> titleText(*textBuf);
    HBufC *buf = NULL;
    if(aItem == ENameItem)
        {
        buf = StringLoader::LoadLC(R_GLX_METADATA_VIEW_TITLE_NSERIES);
        }
    else
        {
        buf = StringLoader::LoadLC(R_GLX_METADATA_VIEW_DESCRIPTION_NSERIES);
        }

    //Launch the text entry editor.
    CGlxTextEntryPopup* popup = CGlxTextEntryPopup::NewL( *buf, textPtr );
    CleanupStack::PopAndDestroy(buf);
    if(aItem == EImgVwrDescriptionItem)
        {
        popup->SetLeftSoftKeyL(ETrue);
        }
    
    //action upon selecting ok from the editor 
    if ( popup->ExecuteLD() == EEikBidOk )
    {
      if(0 != (titleText.Compare(*textBuf)))
      {
        //Modify the MDS and setting list only if the entry is different from previous Item value
        iTextSetter.Zero();
        iTextSetter.Copy(*textBuf);
        EditItemL(aItem,EFalse);
        if( iItemMediaList->Count() > 0 )
        {
          iItemMediaList->SetFocusL(NGlxListDefs::EAbsolute,0);//set focus to first item
          CMPXCollectionPath* path = iItemMediaList->PathLC();
          CMPXCommand* command = NULL;
          //Create the glx command based on the item
          if(aItem == ENameItem)
              {
              command = TGlxCommandFactory::RenameCommandLC(settingsitem->SettingTextL(),
     *path); 
              }
          else
              {
              command = TGlxCommandFactory::SetDescriptionCommandLC(settingsitem->SettingTextL(),
     *path);
              }              
          command->SetTObjectValueL<TAny*>(KMPXCommandGeneralSessionId, static_cast<TAny*>(this));
          //issue command to the medialist which further calls data source to update MDS
          iItemMediaList->CommandL(*command);
          CleanupStack::PopAndDestroy(command);
          CleanupStack::PopAndDestroy(path);
          }
      }         
    }
    CleanupStack::PopAndDestroy( textBuf );
     
    }
// ----------------------------------------------------------------------------
// CGlxImgVwrMetadataContainer::SetDurationLIicenseItemVisibilityL()
// ----------------------------------------------------------------------------
// 
void CGlxImgVwrMetadataContainer::SetDurationLIicenseItemVisibilityL()
    {
    TRACER("CGlxMetadataContainer::SetDurationLIicenseItemVisibilityL()");
    //get the media item.
    const TGlxMedia& item = iItemMediaList->Item(0);
    const CGlxMedia* media = item.Properties();

    //in order to check for video category and drm rights
   
    CAknSettingItem* hiddenItem = NULL;
        
     if( media && media->IsSupported(KMPXMediaDrmProtected))
        {
         if(item.IsDrmProtected())
             {
             hiddenItem = (*SettingItemArray())[EImgVwrlicenseItem];
             //Set the License item visible
             hiddenItem->SetHidden(EFalse);             
             //Required to refresh the listbox when any items visiblity is changed
             this->HandleChangeInItemArrayOrVisibilityL();
             }
        }
    }
//-----------------------------------------------------------------------------
// CGlxImgVwrMetadataContainer::IsLicenseItem
//-----------------------------------------------------------------------------
TBool CGlxImgVwrMetadataContainer::IsLicenseItem()
    {
    TRACER("CGlxMetadataContainer::IsLicenseItem()");
    //Checks the item for DRMProtection.
    if((ListBox()->CurrentItemIndex()== EImgVwrlicenseItem))
        {
        return EFalse;
        }
    return ETrue;
    }
 

//End of file
