// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YRankPanel.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FTempPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString PlayerName;

	UPROPERTY(BlueprintReadWrite)
	int32 IndexInPlayerArray;

	UPROPERTY(BlueprintReadWrite)
	int32 PlayerScore;

	FORCEINLINE bool operator < (const FTempPlayerInfo& Other) const
	{
		return this->PlayerScore > Other.PlayerScore;
	}
};


UCLASS()
class YZKPROJECT_API UYRankPanel : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FTempPlayerInfo CreateTempPlayerInfo(const FString& PlayerName, const int32 IndexInPlayerArray, const int32 PlayerScore)
	{
		return { PlayerName, IndexInPlayerArray, PlayerScore };
	}

	UFUNCTION(BlueprintCallable)
	TArray<FTempPlayerInfo> SortTempPlayerInfoArray(const TArray<FTempPlayerInfo>& InfoArray);
	
};
