// Fill out your copyright notice in the Description page of Project Settings.


#include "YRankPanel.h"

TArray<FTempPlayerInfo> UYRankPanel::SortTempPlayerInfoArray(const TArray<FTempPlayerInfo>& InfoArray)
{
    TArray<FTempPlayerInfo> NewArray = InfoArray;
    NewArray.Sort();
    return NewArray;
}