// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Misc/Paths.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BFL_PTZCamera.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API UBFL_PTZCamera : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "JSONParser")
	static bool ParseData(const FString& JsonString,const FString& delimiter,TArray<FString>& Data);
	
	UFUNCTION(BlueprintCallable, Category = "JSONParser")
	static FString RemoveSpacesFromString(const FString& InputString);

	
};
