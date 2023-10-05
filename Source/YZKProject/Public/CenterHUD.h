// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine/Canvas.h" 
#include "CenterHUD.generated.h"

/**
 * 
 */
UCLASS()
class YZKPROJECT_API ACenterHUD : public AHUD
{
	GENERATED_BODY()
protected:
	// 将被绘制在屏幕中心。
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* CrossHairTexture;

public:
	// HUD绘制的主要调用。
	virtual void DrawHUD() override;
	
};
