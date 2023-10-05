// Fill out your copyright notice in the Description page of Project Settings.


#include "CenterHUD.h"

void ACenterHUD::DrawHUD()
{
    Super::DrawHUD();

    if (CrossHairTexture)
    {
        // �ҳ����ǵĻ��������ĵ㡣
        FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

        // ƫ�������С��һ�룬�Ա����������뻭�����Ķ��롣
        FVector2D CrossHairDrawPosition(Center.X - (CrossHairTexture->GetSurfaceWidth() * 0.5f), Center.Y - (CrossHairTexture->GetSurfaceHeight() * 0.5f));

        // �����ĵ����ʮ��׼�ǡ�
        FCanvasTileItem TileItem(CrossHairDrawPosition, CrossHairTexture->GetResource(), FLinearColor::White);
        TileItem.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(TileItem);
    }
}
