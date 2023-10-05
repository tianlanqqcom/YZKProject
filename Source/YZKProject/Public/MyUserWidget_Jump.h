// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "MyUserWidget_Jump.generated.h"

/**
 * 
 */
class ACharacter;

UCLASS()
class YZKPROJECT_API UMyUserWidget_Jump : public UUserWidget
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable)
	void CallJumpFunctionAndLog();

	ACharacter* GetPlayerCharacter() const;

	UFUNCTION(BlueprintCallable)
	void BeginJump();

	UFUNCTION(BlueprintCallable)
	void EndJump();
};
