// Fill out your copyright notice in the Description page of Project Settings.


#include "BFL_PTZCamera.h"
#include "Json.h"
#include "JsonUtilities.h"
bool UBFL_PTZCamera::ParseData(const FString& JsonString,const FString& delimiter, TArray<FString>& Data)
{
	if (JsonString.IsEmpty()){return false;}
	JsonString.ParseIntoArray(Data, *delimiter);
	return Data.Num() > 0;
}

FString UBFL_PTZCamera::RemoveSpacesFromString(const FString& InputString)
{
	FString OutputString = InputString;  // 创建一个输出字符串

	// 使用 ReplaceInline() 方法将所有空格和换行替换为空字符串
	OutputString.ReplaceInline(TEXT("\n"), TEXT(""));  // 去除换行符
	OutputString.ReplaceInline(TEXT("\r"), TEXT(""));  // 去除回车符
	OutputString.ReplaceInline(TEXT(" "), TEXT(""));   // 去除空格

	return OutputString;
}

