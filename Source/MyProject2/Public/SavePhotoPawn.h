// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SavePhotoPawn.generated.h"

UCLASS()
class MYPROJECT2_API ASavePhotoPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASavePhotoPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// Scene capture component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Capture")
	USceneCaptureComponent2D* SceneCaptureComponent;

	// Timer for capture function
	FTimerHandle CaptureTimerHandle;

	// Capture function
	void CaptureImage();

	UFUNCTION(BlueprintCallable,Category="Capture")
	// Save image function
	void SaveImage(bool Debug);

	static void Print(const FString& Target);

};
