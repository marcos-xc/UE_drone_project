// Fill out your copyright notice in the Description page of Project Settings.


#include "SavePhotoPawn.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
// #include "Engine/TextureRenderTarget2D.h"
// #include "Engine/SceneCapture2D.h"
// #include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
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
// void ASavePhotoPawn::CaptureImage()
// {
// 	// Call the capture function
// 	SaveImage(true);
// }

// Save the captured image to a file
// void ASavePhotoPawn::SaveImage(bool Debug)
// {
// 	// 创建支持HDR的渲染目标
// 	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>(this);
// 	RenderTarget->InitCustomFormat(1920, 1080, PF_A16B16G16R16, false); // 使用16位浮点格式
// 	RenderTarget->TargetGamma = 2.2f; // 设置目标Gamma值
// 	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
//
// 	// 确保后处理设置生效
// 	SceneCaptureComponent->PostProcessSettings.bOverride_SceneFringeIntensity = true;
// 	SceneCaptureComponent->PostProcessSettings.SceneFringeIntensity = 0.0f;
// 	SceneCaptureComponent->PostProcessBlendWeight = 1.0f;
// 	//SceneCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorHDR;
// 	SceneCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR; // 使用后处理后的颜色
//
// 	// 设置渲染目标并捕获
// 	SceneCaptureComponent->TextureTarget = RenderTarget;
// 	SceneCaptureComponent->CaptureScene();
//
// 	// 读取像素数据（保持线性颜色空间）
// 	TArray<FColor> Bitmap;
// 	FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
// 	if (RTResource)
// 	{
// 		RTResource->ReadPixels(Bitmap, FReadSurfaceDataFlags(), FIntRect(0, 0, RenderTarget->SizeX, RenderTarget->SizeY));
//         
// 		// 转换到sRGB颜色空间（如果需要）
// 		for (FColor& Pixel : Bitmap)
// 		{
// 			Pixel = FLinearColor(Pixel).ToFColor(true); // 转换为sRGB
// 		}
//
// 		// 保存为PNG
// 		FString Filename = FPaths::ProjectSavedDir() / TEXT("CapturedImage.png");
// 		FFileHelper::CreateBitmap(*Filename, RenderTarget->SizeX, RenderTarget->SizeY, Bitmap.GetData());
// 		
//         
// 		if (Debug) Print(Filename);
// 	}
// }
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





void ASavePhotoPawn::SaveImage(const FString& SavePath, const FString& FileName, bool bOverride, bool Debug)
{
    // 如果不在 Game Thread，则切换到 Game Thread
    if (!IsInGameThread())
    {
        UE_LOG(LogTemp, Warning, TEXT("SaveImage called from a non-game thread. Switching to Game Thread."));
        AsyncTask(ENamedThreads::GameThread, [this, SavePath, FileName, bOverride, Debug]()
        {
            this->SaveImage(SavePath, FileName, bOverride, Debug);
        });
        return;
    }

    // 检查 SceneCaptureComponent 是否有效
    if (!SceneCaptureComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("SceneCaptureComponent is null!"));
        return;
    }

    // 创建支持HDR的渲染目标
    UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>(this);
    if (!RenderTarget)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create RenderTarget!"));
        return;
    }

    // 使用 PF_FloatRGBA 格式
    RenderTarget->InitCustomFormat(1920, 1080, PF_FloatRGBA, false);
    RenderTarget->TargetGamma = 2.2f;
    RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;

    // 确保后处理设置生效
    SceneCaptureComponent->PostProcessSettings.bOverride_SceneFringeIntensity = true;
    SceneCaptureComponent->PostProcessSettings.SceneFringeIntensity = 0.0f;
    SceneCaptureComponent->PostProcessBlendWeight = 1.0f;
    SceneCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorHDR;

    // 设置渲染目标并捕获
    SceneCaptureComponent->TextureTarget = RenderTarget;
    SceneCaptureComponent->CaptureScene();

    // 检查渲染目标是否有效
    if (!SceneCaptureComponent->TextureTarget)
    {
        UE_LOG(LogTemp, Error, TEXT("TextureTarget is null!"));
        return;
    }

    // 生成完整路径
    FString FullFilePath = FPaths::Combine(SavePath, FileName + TEXT(".png"));
    if (FullFilePath.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("File path is empty!"));
        return;
    }

    if (bOverride)
    {
        // 获取文件夹中的所有 PNG 文件
        TArray<FString> PNGFiles;
        IFileManager& FileManager = IFileManager::Get();
        const FString SearchPath = FPaths::Combine(SavePath, TEXT("*.bmp"));
        FileManager.FindFiles(PNGFiles, *SearchPath, true, false);

        if (PNGFiles.Num() > 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("Found PNG files, deleting..."));
            for (const FString& PNGFile : PNGFiles)
            {
                FString FilePath = FPaths::Combine(SavePath, PNGFile);
                if (FileManager.Delete(*FilePath))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Deleted file: %s"), *FilePath);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Failed to delete file: %s"), *FilePath);
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No PNG files found."));
        }
    }

    // 读取像素数据
    TArray<FFloat16Color> HDRBitmap;
    FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
    if (!RTResource)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get RenderTargetResource!"));
        return;
    }

    RTResource->ReadFloat16Pixels(HDRBitmap);

    // 如果需要保存为LDR PNG，将HDR数据转换为8位RGBA
    TArray<FColor> LDRBitmap;
    LDRBitmap.Reserve(HDRBitmap.Num());
    for (const FFloat16Color& HDRPixel : HDRBitmap)
    {
        FLinearColor LinearColor(HDRPixel.R, HDRPixel.G, HDRPixel.B, HDRPixel.A);
        LDRBitmap.Add(LinearColor.ToFColor(true));
    }

    // 保存为PNG
    if (FFileHelper::CreateBitmap(*FullFilePath, RenderTarget->SizeX, RenderTarget->SizeY, LDRBitmap.GetData()))
    {
        if (Debug)
        {
            UE_LOG(LogTemp, Warning, TEXT("Saved image to: %s"), *FullFilePath);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save image to: %s"), *FullFilePath);
    }
}




void ASavePhotoPawn::Print(const FString& Target)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::MakeRandomColor(),Target);
	}
}