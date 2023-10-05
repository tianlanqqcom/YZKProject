// Fill out your copyright notice in the Description page of Project Settings.

#include "MyUserWidget_Jump.h"
#include "GameFramework/Character.h"

void UMyUserWidget_Jump::CallJumpFunctionAndLog()
{
#if WITH_EDITOR

	// FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("start"), TEXT("callJumptFunction"));
    check(GEngine != nullptr);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Attemp Jump"));
#endif

    auto NowCharacter = GetPlayerCharacter();

    if(!NowCharacter)
    {
#if WITH_EDITOR
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No Character"));
#endif
        return;
    }


    NowCharacter->bPressedJump = true;
}

ACharacter* UMyUserWidget_Jump::GetPlayerCharacter() const
{
    APlayerController* PlayerController = GetOwningPlayer();
    if (PlayerController)
    {
	    if (APawn* PlayerPawn = PlayerController->GetPawn())
        {
            return Cast<ACharacter>(PlayerPawn);
        }
    }
    return nullptr;
}

void UMyUserWidget_Jump::BeginJump()
{
#if WITH_EDITOR
    check(GEngine != nullptr);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Begin Jump"));
#endif

    auto NowCharacter = GetPlayerCharacter();
    if (!NowCharacter)
    {
#if WITH_EDITOR
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No Character"));
#endif

        return;
    }

    NowCharacter->bPressedJump = true;
}

void UMyUserWidget_Jump::EndJump()
{
#if WITH_EDITOR
    check(GEngine != nullptr);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("End Jump"));
#endif

    auto NowCharacter = GetPlayerCharacter();
    if (!NowCharacter)
    {
#if WITH_EDITOR
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No Character"));
#endif

        return;
    }

    NowCharacter->bPressedJump = false;
}
