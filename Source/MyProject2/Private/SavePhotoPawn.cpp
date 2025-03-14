// Fill out your copyright notice in the Description page of Project Settings.


#include "SavePhotoPawn.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
// #include "Engine/TextureRenderTarget2D.h"
// #include "Engine/SceneCapture2D.h"
// #include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
// #include "ImageUtils.h"
// Sets default values
ASavePhotoPawn::ASavePhotoPawn()
{
	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;
    
	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	SceneCaptureComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASavePhotoPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASavePhotoPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASavePhotoPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}



// Capture the image and save it
void ASavePhotoPawn::CaptureImage()
{
	// Call the capture function
	SaveImage(true);
}

// Save the captured image to a file
void ASavePhotoPawn::SaveImage(bool Debug)
{
	// 创建支持HDR的渲染目标
	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->InitCustomFormat(1920, 1080, PF_A16B16G16R16, false); // 使用16位浮点格式
	RenderTarget->TargetGamma = 2.2f; // 设置目标Gamma值
	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;

	// 确保后处理设置生效
	SceneCaptureComponent->PostProcessSettings.bOverride_SceneFringeIntensity = true;
	SceneCaptureComponent->PostProcessSettings.SceneFringeIntensity = 0.0f;
	SceneCaptureComponent->PostProcessBlendWeight = 1.0f;
	//SceneCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorHDR;
	SceneCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR; // 使用后处理后的颜色

	// 设置渲染目标并捕获
	SceneCaptureComponent->TextureTarget = RenderTarget;
	SceneCaptureComponent->CaptureScene();

	// 读取像素数据（保持线性颜色空间）
	TArray<FColor> Bitmap;
	FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
	if (RTResource)
	{
		RTResource->ReadPixels(Bitmap, FReadSurfaceDataFlags(), FIntRect(0, 0, RenderTarget->SizeX, RenderTarget->SizeY));
        
		// 转换到sRGB颜色空间（如果需要）
		for (FColor& Pixel : Bitmap)
		{
			Pixel = FLinearColor(Pixel).ToFColor(true); // 转换为sRGB
		}

		// 保存为PNG
		FString Filename = FPaths::ProjectSavedDir() / TEXT("CapturedImage.png");
		FFileHelper::CreateBitmap(*Filename, RenderTarget->SizeX, RenderTarget->SizeY, Bitmap.GetData());
		
        
		if (Debug) Print(Filename);
	}
}
//
// void ASavePhotoPawn::SaveImage()
// {
// 	// 创建一个合适的 Render Target
// 	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>(this);
// 	RenderTarget->InitCustomFormat(1920, 1080, PF_B8G8R8A8, false);
// 	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
// 	// RenderTarget->bHDR = false;
//
// 	// 设置 Scene Capture 组件
// 	SceneCaptureComponent->TextureTarget = RenderTarget;
// 	SceneCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
// 	SceneCaptureComponent->bEnableClipPlane = false;
//
// 	// 捕获场景
// 	SceneCaptureComponent->CaptureScene();
// 	FlushRenderingCommands();  // 确保渲染线程完成捕捉
//
// 	// 获取 Render Target 资源
// 	FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
// 	if (!RenderTargetResource) return;
//
// 	// 读取像素数据
// 	int32 Width = RenderTarget->SizeX;
// 	int32 Height = RenderTarget->SizeY;
// 	TArray<FColor> Bitmap;
// 	Bitmap.SetNumUninitialized(Width * Height);
// 	RenderTargetResource->ReadPixels(Bitmap);
//
// 	// 处理 Gamma 变换
// 	for (FColor& Pixel : Bitmap)
// 	{
// 		Pixel.R = FMath::Pow(Pixel.R / 255.0f, 1.0f / 2.2f) * 255;
// 		Pixel.G = FMath::Pow(Pixel.G / 255.0f, 1.0f / 2.2f) * 255;
// 		Pixel.B = FMath::Pow(Pixel.B / 255.0f, 1.0f / 2.2f) * 255;
// 	}
//
// 	// 保存图像
// 	FString Filename = FPaths::ProjectSavedDir() / TEXT("CapturedImage.png");
// 	FFileHelper::CreateBitmap(*Filename, Width, Height, Bitmap.GetData());
//
// 	UE_LOG(LogTemp, Log, TEXT("Image saved: %s"), *Filename);
// }


void ASavePhotoPawn::Print(const FString& Target)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::MakeRandomColor(),Target);
	}
}