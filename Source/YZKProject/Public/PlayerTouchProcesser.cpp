#include "PlayerTouchProcesser.h"
#include "YNFPPCharacter.h"

FPlayerTouchProcesser::FPlayerTouchProcesser()
{
}

FPlayerTouchProcesser::FPlayerTouchProcesser(int32 ViewX, int32 ViewY):ViewX(ViewX),ViewY(ViewY)
{
	for(ETouchIndex::Type Index = ETouchIndex::Touch1; Index <= ETouchIndex::MAX_TOUCHES; Index = static_cast<ETouchIndex::Type>(Index + 1))
	{
		LocalInput[Index] = { FVector(0,0,0), false };
	}
}

void FPlayerTouchProcesser::FingerPressed(ETouchIndex::Type FingerIndex, FVector Location)
{
	LocalInput[FingerIndex].Location = Location;
	if(FingerIndex <= ETouchIndex::Touch10)
	{
		if(!bAlreadyHasRotationInput && Location.X > (ViewX >> 1))
		{
			LocalInput[FingerIndex].bHasPressed = true;
			bAlreadyHasRotationInput = true;
			ActiveInput = FingerIndex;
		}
	}
}

FTouchResult FPlayerTouchProcesser::FingerMoved(ETouchIndex::Type FingerIndex, FVector Location)
{
	FTouchResult Result{};

	if(LocalInput[FingerIndex].bHasPressed && LocalInput[FingerIndex].Location.X > (ViewX >> 1) && Location.X > (ViewX >> 1))
	{
		Result.DeltaX = Location.X - LocalInput[FingerIndex].Location.X;
		Result.DeltaY = Location.Y - LocalInput[FingerIndex].Location.Y;
	}
	else
	{
		Result.DeltaX = 0.f;
		Result.DeltaY = 0.f;
	}

	LocalInput[FingerIndex].Location = Location;
	return Result;
}

void FPlayerTouchProcesser::FingerRelease(ETouchIndex::Type FingerIndex, FVector Location)
{
	LocalInput[FingerIndex].bHasPressed = false;
	LocalInput[FingerIndex].Location = Location;

	if(FingerIndex == ActiveInput)
	{
		bAlreadyHasRotationInput = false;
		ActiveInput = ETouchIndex::MAX_TOUCHES;
	}
}




