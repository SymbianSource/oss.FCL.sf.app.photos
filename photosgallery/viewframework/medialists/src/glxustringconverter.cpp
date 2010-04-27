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
* Description:    UString converter implementation class that converts symbian data 
*                types to UString types
*
*/




#include "glxustringconverter.h"
#include "glxmediageneraldefs.h"

#include <AknUtils.h> 
#include <aknlocationed.h>      // CAknLocationEditor
#include <avkon.rsg>
#include <utf.h>                // CnvUtfConverter
#include <osn/osnnew.h>             // for new(EMM) 
#include <StringLoader.h>
#include <glxtracer.h> 
#include <glxmetadatadialog.rsg>
#include <lbsposition.h>        // for TCoordinate
#include <glxsettingsmodel.h>   // For Cenrep Keys

const TInt KBytesInKB = 1024;
const TInt KBytesInMB = 1024 * 1024;
const TInt KBytesInGB = 1024 * 1024 * 1024;
_LIT( KBlankText, " " );


// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxUStringConverter::CGlxUStringConverter()
    {
    TRACER("CGlxUStringConverter::CGlxUStringConverter()");
    
    }

// -----------------------------------------------------------------------------
// Two phase Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxUStringConverter* CGlxUStringConverter::NewL()
    {
    TRACER("CGlxUStringConverter::NewL");
    CGlxUStringConverter* self =  CGlxUStringConverter::NewLC();
    
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Two phase Constructor
// -----------------------------------------------------------------------------
//   
CGlxUStringConverter* CGlxUStringConverter::NewLC()
    {
    TRACER("CGlxUStringConverter::NewLC");
    CGlxUStringConverter* self = new (ELeave)  CGlxUStringConverter();
    CleanupStack::PushL(self );
    return self;
    }

CGlxUStringConverter::~CGlxUStringConverter()
    {
    
    }
// -----------------------------------------------------------------------------
// Converts the TDesC into UString type
// -----------------------------------------------------------------------------
//  
EXPORT_C void CGlxUStringConverter::AsStringL(const TGlxMedia& aMedia, const TMPXAttribute& aAttribute, TInt aFormatString, HBufC*& aString ) const
    {
    TRACER("CGlxUStringConverter::AsStringL");
    const CGlxMedia* media = aMedia.Properties();
    if( media )
        {
        switch( media->AttributeTypeId(aAttribute) )
    		{
    		case EMPXTypeText:
                {
                aString = ( media->ValueText(aAttribute) ).Alloc();
                }
           break;


    		case EMPXTypeTObject:
    			{
    			if( aAttribute == KMPXMediaGeneralDate )
    				{
    				TTime date(0);
    				if( aFormatString == R_QTN_DATE_USUAL_WITH_ZERO )
                        {
                        HBufC* dateString = HBufC::NewLC(KMaxLongDateFormatSpec);
                        TPtr dateStringPtr (dateString->Des());
                        media->GetValueTObject(date,KMPXMediaGeneralDate);
                        HBufC* dateFormat = CCoeEnv::Static()->AllocReadResourceLC
                            ( R_QTN_DATE_USUAL_WITH_ZERO );
                        date.FormatL( dateStringPtr , *dateFormat );
                        CleanupStack::PopAndDestroy(dateFormat);
                        AknTextUtils::LanguageSpecificNumberConversion( dateStringPtr );
                        
                        HBufC* timeString = HBufC::NewLC(KMaxTimeFormatSpec);
                        TPtr timeStringPtr (timeString->Des());
                        media->GetValueTObject(date,KMPXMediaGeneralDate);
                        HBufC* timeFormat = CCoeEnv::Static()->AllocReadResourceLC
                             ( R_QTN_TIME_USUAL_WITH_ZERO );
                        date.FormatL(  timeStringPtr , *timeFormat );
                        CleanupStack::PopAndDestroy(timeFormat);
                        AknTextUtils::LanguageSpecificNumberConversion( timeStringPtr );
                         
                        HBufC* dateAndTime = HBufC::NewLC(dateStringPtr.Length()+timeStringPtr.Length()+1);
                        TPtr dateAndTimePtr (dateAndTime->Des());
                        dateAndTimePtr = dateStringPtr;
                        dateAndTimePtr.Append( KBlankText );
                        dateAndTimePtr.Append(timeStringPtr);
                        aString = dateAndTime->Alloc();                       
                        CleanupStack::PopAndDestroy(dateAndTime);
                        CleanupStack::PopAndDestroy(timeString);
                        CleanupStack::PopAndDestroy(dateString);
                        }
    				else if( aFormatString == R_QTN_TIME_USUAL_WITH_ZERO)
    					{
    					TBuf<20> timeString;
    					media->GetValueTObject(date,KMPXMediaGeneralDate);
    					HBufC* timeFormat = CCoeEnv::Static()->AllocReadResourceLC
    						( R_QTN_TIME_USUAL_WITH_ZERO );
    					
    					date.FormatL(  timeString , *timeFormat );
    					CleanupStack::PopAndDestroy(timeFormat);
    					AknTextUtils::LanguageSpecificNumberConversion( timeString );
    					aString = timeString.Alloc();
    					}
                    else if( aFormatString == R_QTN_DATE_USUAL)
                        {
                        TBuf<20> dateString;
                        media->GetValueTObject(date,KMPXMediaGeneralDate);
                        HBufC* dateFormat = CCoeEnv::Static()->AllocReadResourceLC
                            ( R_QTN_DATE_USUAL );
                        
                        date.FormatL(  dateString , *dateFormat );
                        CleanupStack::PopAndDestroy(dateFormat);
                        AknTextUtils::LanguageSpecificNumberConversion( dateString );
                        aString = dateString.Alloc();
                        }

    				}
    			else if( aAttribute == KMPXMediaGeneralSize )
    				{
                    GetFormattedItemSizeL(*media, aString);
    				}
    			else if( aAttribute == KGlxMediaGeneralLocation )
    				{
    				TCoordinate coordinate;
    				if(aMedia.GetCoordinate( coordinate ))
    					{
    					GetFormatLocationL(coordinate, aString);
    					}
    				else
    				    {
    				    HBufC* locationTextBuf = HBufC::NewLC(1); 
    				    TPtr ptr(locationTextBuf->Des());
    				    _LIT(KFormat," ");
     				    ptr.Append(KFormat);
    				    aString = locationTextBuf->Alloc();
    				    CleanupStack::PopAndDestroy(locationTextBuf);
    				    }
    				}
    			else if( aAttribute == KMPXMediaGeneralDuration )
    				{
    				TReal32 duration( 0 );
    				aMedia.GetDuration( duration );
    				TBuf<20> timeBuf(0);
    				if(duration)
    					{
    					GetFormattedDurationL(duration, aString);	
    					}
				    }
    			else if( aAttribute == KGlxMediaGeneralDimensions )
    				{
    				TSize resolution;
    				if(aMedia.GetDimensions(resolution))
    					{
    					GetFormatResolutionL(resolution, aString);
    					}
    				}
    			else 
    				{
    				// no implementation
    				}
    			}
    		break;
    		default:
    		break;
    		}
        }
    }
 

// ---------------------------------------------------------------------------
// Returns the item size in the required format
// ---------------------------------------------------------------------------
//
void CGlxUStringConverter::GetFormattedItemSizeL(const CGlxMedia& aMedia, HBufC*& aString) const
 	{
 	TRACER("CGlxUStringConverter::GetFormattedItemSizeL");
    TUint size(0);
    if(aMedia.GetValueTObject( size, KMPXMediaGeneralSize ))
        {
        if(size >= KBytesInGB)
            {
            TInt gbSize = size / KBytesInGB ; // Size in GB
            aString = StringLoader::LoadL(R_QTN_SIZE_GB, gbSize);
            }
        else if(size >= KBytesInMB)
            {
            TInt mbSize = size / KBytesInMB ; // Size in MB
        	aString = StringLoader::LoadL(R_QTN_SIZE_MB, mbSize);
            }
        else if(size >= KBytesInKB)
            {
            aString = HBufC::NewL(64);
            TInt kBsize = size / KBytesInKB;  // bytes to kB
            HBufC* textSizeKb = CCoeEnv::Static()->AllocReadResourceLC( R_QTN_SIZE_KB );
            TPtr ptr = aString->Des();
            TPtr sizePtr = textSizeKb->Des();
            StringLoader::Format(ptr,sizePtr, -1, kBsize);
            CleanupStack::PopAndDestroy(textSizeKb);
            }
        else
			{
			HBufC* textSizeB = CCoeEnv::Static()->AllocReadResourceLC( R_QTN_SIZE_B );
			aString = HBufC::NewL(64);
			TPtr ptr = aString->Des();
            TPtr sizePtr = textSizeB->Des();
            StringLoader::Format(ptr, sizePtr, -1, size);
            CleanupStack::PopAndDestroy(textSizeB);
            }
        if(aString)
            {
            TPtr ptr = aString->Des();
            AknTextUtils::LanguageSpecificNumberConversion(ptr);
            }
        }
 	}
 
// ---------------------------------------------------------------------------
// Returns the item location in the required format
// ---------------------------------------------------------------------------
//	
void CGlxUStringConverter::GetFormatLocationL(const TCoordinate& aCoordinate, HBufC*& aString ) const
	{
	TRACER("CGlxUStringConverter::GetFormatLocationL");
    CGlxSettingsModel* settingsModel = CGlxSettingsModel::InstanceL();
    CleanupClosePushL(*settingsModel);
    TBool showGeoCoordinates = settingsModel->ShowGeoCoordinatesInDisplay();
    CleanupStack::PopAndDestroy(settingsModel);  

	if (showGeoCoordinates)
		{
		//Show geocoordinates
		TPosition origin;
		origin.SetCoordinate(aCoordinate.Latitude(),aCoordinate.Longitude());
		HBufC* latBuf = CAknLocationEditor::DisplayableLocationL( origin, CAknLocationEditor::ELatitudeOnly );
		HBufC* longBuf = CAknLocationEditor::DisplayableLocationL( origin, CAknLocationEditor::ELongitudeOnly );
		HBufC* combinedLocationTextBuf = HBufC::NewLC( latBuf->Length() + longBuf->Length() + 2 ); 
		TPtr ptr(combinedLocationTextBuf->Des());
		_LIT(KFormat,", ");
		ptr.Append(*latBuf);
		ptr.Append(KFormat);
		ptr.Append(*longBuf);
	    aString = combinedLocationTextBuf->Alloc();
		CleanupStack::PopAndDestroy(combinedLocationTextBuf);
	    if( latBuf )
	        {
	        delete latBuf;
	        latBuf = NULL;
	        }
	    if( longBuf )
	        {
	        delete longBuf;
	        longBuf = NULL;
	        }
		}
	else
		{
		//Hide geocoordinates
		TBuf<KMaxInfoName> noLocationTextBuf;
		noLocationTextBuf.AppendNum(0);
		noLocationTextBuf.AppendNum(0);
	    aString = noLocationTextBuf.Alloc();
        }
    }
    
// ---------------------------------------------------------------------------
// Returns the item duration in the required format
// ---------------------------------------------------------------------------
//
 void CGlxUStringConverter::GetFormattedDurationL(const TReal& aDuration , HBufC*& aString ) const
 	{
 	TRACER("CGlxUStringConverter::GetFormattedDurationL");
 	TInt resourceId = R_QTN_TIME_DURAT_LONG;
	if( aDuration <= 3600 ) //60 seconds *60 minutes
		{
		// minute:seconds format
		resourceId = R_QTN_TIME_DURAT_MIN_SEC;
		}
	
	// This class does not have access to a CEikonEnv and hence 
	// pls ignore the code scanner warning - Using CEikonEnv::Static
	HBufC* timeFormat = CEikonEnv::Static()->AllocReadResourceLC( resourceId );

	TTime time( 0 );
	TBuf<20> timeBuf;
	time += TTimeIntervalSeconds ( aDuration );
	time.FormatL( timeBuf , *timeFormat );
    //to convert between arabic-indic digits and european digits.
    //based on existing language setting.
    AknTextUtils::LanguageSpecificNumberConversion(timeBuf);
	CleanupStack::PopAndDestroy(timeFormat);
	aString = timeBuf.Alloc();
 	}

// ---------------------------------------------------------------------------
// Returns the item resolution in the required format
// ---------------------------------------------------------------------------
//	
void CGlxUStringConverter::GetFormatResolutionL(const TSize& aSize, HBufC*& aString  ) const
    {
    TRACER("CGlxUStringConverter::GetFormatResolutionL");
    CArrayFix<TInt>* array = new (ELeave) CArrayFixFlat<TInt>(2);
	CleanupStack::PushL(array);
	array->AppendL(aSize.iWidth);
	array->AppendL(aSize.iHeight);
	
	//@todo: to be included when the metadata dialog's resource file is ready.
    HBufC* formatString = 
    StringLoader::LoadL(R_GLX_METADATA_VIEW_RESOLUTION_DETAIL_NSERIES,*array,CCoeEnv::Static());
	CleanupStack::PopAndDestroy(array);
	aString = formatString->Alloc();
	if( formatString )
	    {
    	delete formatString;
    	formatString = NULL;
	    }
    }
