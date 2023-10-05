// Fill out your copyright notice in the Description page of Project Settings.


#include "CenterHUD.h"

void ACenterHUD::DrawHUD()
{
    Super::DrawHUD();

    if (CrossHairTexture)
    {
        // 找出我们的画布的中心点。
        FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

        // 偏移纹理大小的一半，以便纹理中心与画布中心对齐。
        FVector2D CrossHairDrawPosition(Center.X - (CrossHairTexture->GetSurfaceWidth() * 0.5f), Center.Y - (CrossHairTexture->GetSurfaceHeight() * 0.5f));

        // 在中心点绘制十字准星。
        FCanvasTileItem TileItem(CrossHairDrawPosition, CrossHairTexture->GetResource(), FLinearColor::White);
        TileItem.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(TileItem);
    }
}
