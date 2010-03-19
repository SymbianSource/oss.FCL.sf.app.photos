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

#include <mpxcollectionpath.h>
#include <mglxmedialist.h>
#include <glxcommandfactory.h>
#include <GlxCommandHandlerRotate.h>
#include <QDebug>
#include <ExifRead.h>

GlxCommandHandlerRotate::GlxCommandHandlerRotate()
{
    qDebug("GlxCommandHandlerRotate::GlxCommandHandlerRotate() ");
}

GlxCommandHandlerRotate::~GlxCommandHandlerRotate()
{
    qDebug("GlxCommandHandlerRotate::~GlxCommandHandlerRotate() ");
}

CMPXCommand* GlxCommandHandlerRotate::CreateCommandL(TInt aCommandId, MGlxMediaList& aMediaList, TBool& aConsume) const 
{
	Q_UNUSED(aCommandId);
	Q_UNUSED(aConsume);
    qDebug("GlxCommandHandlerRotate::CreateCommandL");
    return NULL;
}

void GlxCommandHandlerRotate::DoExecuteCommandL(TInt aCommandId, MGlxMediaList& aMediaList, TBool& aConsume)
{
	Q_UNUSED(aCommandId);
	Q_UNUSED(aConsume);
    qDebug("GlxCommandHandlerRotate::DoExecuteCommandL");
    const TGlxMedia& item = aMediaList.Item( aMediaList.FocusIndex() );
	TFileName uri;
	uri.Copy(item.Uri());
	TRAPD(err,RotateImageL(uri));
	if(err != KErrNone) {
		qDebug("GlxCommandHandlerRotate::DoExecuteCommandL Exif Update failed");

	}
}
void GlxCommandHandlerRotate::RotateImageL(TFileName aFileToBeRotated)
{
	//Start an IFS session
	//File system 
    User::LeaveIfError(iFs.Connect());
	//first initialize the Exif Writer which is responsible for reading and writing the Exif Data
	//Will leave here itself in cases where the Exif data is absent
	InitializeExifWriterL(aFileToBeRotated);
	//read the Orientation tag stored in  the Exif Data
	TUint16 initialOrientation = ReadImageOrientationL();
	//as the image is rotated to 90 degrees clockwise calculate the new orientation by adding that angle
	TUint16 finalOrientation = CalculateFinalOrientationL(initialOrientation);
	// Set the Final Orientation on the file
	SetImageOrientationL(finalOrientation);
	// Clear the sessions acquired
	DestroyExifWriter();
	//close the File Session
	iFs.Close();
}

void GlxCommandHandlerRotate::InitializeExifWriterL(TFileName aFileToBeRotated)
{
	User::LeaveIfError(iFileHandle.Open(iFs,
                    aFileToBeRotated, EFileWrite));
	TInt filesize;
    User::LeaveIfError(iFileHandle.Size(filesize));
	iExifData = HBufC8::NewL(filesize);
	TPtr8 ptr(iExifData->Des());
    User::LeaveIfError(iFileHandle.Read(ptr));
   	iExifWriter = CExifModify::NewL(*iExifData,CExifModify::EModify,CExifModify::ENoJpegParsing);
}

TUint16 GlxCommandHandlerRotate::ReadImageOrientationL()
{
	TUint16 initialOrientation;
	const CExifRead* exifReader = iExifWriter->Reader(); //not owned
    User::LeaveIfError(exifReader->GetOrientation(initialOrientation));
	qDebug("GlxCommandHandlerRotate::ReadImageOrientationL initial orientation = %d", initialOrientation);
	return (initialOrientation);
}

TUint16 GlxCommandHandlerRotate::CalculateFinalOrientationL(TUint16 aInitialOrientation)
{
	/*
     * possible orientation state with angles for rotation
     * Possible Angles 0 - 90 - 180 - 270
     * Possible states 1 - 8 - 3 - 6 without a Flip
     * Possible states 2 - 7 - 4 - 5 when Flip is on
     */
	TUint16 finalOrientation = aInitialOrientation;
	if(aInitialOrientation >8 ) {
		//invalid orientation passed Leave
        User::Leave(KErrCorrupt);
    }
	TInt rotOffset = 1;
	TInt isOrientationOdd = aInitialOrientation % 2;
	TInt initStateIndex = 0;
    TInt finalStateIndex = 0;
    //Setting the orientation states for the initial unflipped orientation combinations
    TInt orientationStateArray[] = {1,8,3,6};
    //Seting the index for current orientation
    if(aInitialOrientation < 3)
        {
        initStateIndex = 0;
        }
    else if(aInitialOrientation >= 3 && aInitialOrientation < 5)
        {
        initStateIndex = 2;
        }
    else if(aInitialOrientation >= 5 && aInitialOrientation < 7)
        {
        initStateIndex = 3;
        }
    else if(aInitialOrientation >= 7 && aInitialOrientation <= 8)
        {
        initStateIndex = 1;
        }
    //Calculating the final orientation using the cyclic orientationStateArray. 
    //folding final index so that it behaves like a cyclic machine 
    finalStateIndex = (initStateIndex+rotOffset)%4;
    finalOrientation = orientationStateArray[finalStateIndex];
    //Checking if a Flip was present 
    if(aInitialOrientation>4 && isOrientationOdd )
        {
        finalOrientation -= 1;
        }
    if(aInitialOrientation<5 && !isOrientationOdd)
        {
        finalOrientation += 1;
        }
	qDebug("GlxCommandHandlerRotate::CalculateFinalOrientationL final orientation = %d", finalOrientation);
    return finalOrientation;
}

void GlxCommandHandlerRotate::SetImageOrientationL(TUint16 aFinalOrientation)
{
	iExifWriter->SetOrientationL(aFinalOrientation);
	HBufC8* modifiedexifData=NULL;
	modifiedexifData = iExifWriter->WriteDataL(iExifData->Des());
	User::LeaveIfError(iFileHandle.Write(0,modifiedexifData->Des()));
    delete modifiedexifData;	
}

void GlxCommandHandlerRotate::DestroyExifWriter()
{
	iFileHandle.Close();
	if(iExifData != NULL)
        {
        delete iExifData;
        iExifData = NULL;
        }
    if(iExifWriter != NULL)
        {
        delete iExifWriter;
        iExifWriter = NULL;
        } 
}
