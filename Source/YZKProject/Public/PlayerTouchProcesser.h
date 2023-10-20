#pragma once
#include <unordered_map>

#include "CoreMinimal.h"

struct FTouchInfo
{
	FVector Location = FVector(0,0,0);
	bool bHasPressed = false;
};

struct FTouchResult
{
	float DeltaX;
	float DeltaY;
};
class FPlayerTouchProcesser
{
	std::unordered_map<ETouchIndex::Type, FTouchInfo> LocalInput;
	bool bAlreadyHasRotationInput = false;
	ETouchIndex::Type ActiveInput = ETouchIndex::MAX_TOUCHES;
	int32 ViewX;
	int32 ViewY;
public:
	FPlayerTouchProcesser();

	FPlayerTouchProcesser(int32 ViewX, int32 ViewY);

	void FingerPressed(ETouchIndex::Type FingerIndex, FVector Location);

	FTouchResult FingerMoved(ETouchIndex::Type FingerIndex, FVector Location);

	void FingerRelease(ETouchIndex::Type FingerIndex, FVector Location);
	
};
