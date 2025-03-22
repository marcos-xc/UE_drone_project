// Fill out your copyright notice in the Description page of Project Settings.


#include "SavePhotoPawn.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Async/Async.h"
#include "RenderCommandFence.h"
#include "RHI.h"
#include "RHIResources.h"
#include "RenderGraphUtils.h"
// #include "Engine/TextureRenderTarget2D.h"
// #include "Engine/SceneCapture2D.h"
// #include "Kismet/GameplayStatics.h"
#include <string>
#include "Kismet/KismetRenderingLibrary.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"
// #include "ImageUtils.h"
// Sets default values
ASavePhotoPawn::ASavePhotoPawn()
{
	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;
    
	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	SceneCaptureComponent->SetupAttachment(RootComponent);

    RenderTarget = nullptr;
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





#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"

// 复制这段到你的 ASavePhotoPawn.cpp
void ASavePhotoPawn::SaveImage(const FString& SavePath, const FString& FileName, bool bOverride, bool Debug)
{
    if (!IsInGameThread())
    {
        // 如果在非GameThread调用，切回GameThread
        AsyncTask(ENamedThreads::GameThread, [this, SavePath, FileName, bOverride, Debug]()
        {
            this->SaveImage(SavePath, FileName, bOverride, Debug);
        });
        return;
    }

    if (!SceneCaptureComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("SceneCaptureComponent is null!"));
        return;
    }

    // 与BMP版一致的分辨率 & 浮点格式
    const int32 Width = 1280;
    const int32 Height = 720;

    // 如果还没创建 RenderTarget，就用 PF_FloatRGBA
    if (!RenderTarget)
    {
        RenderTarget = NewObject<UTextureRenderTarget2D>(this);
        if (!RenderTarget)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create RenderTarget!"));
            return;
        }
        // 与BMP版保持一致：浮点HDR格式
        RenderTarget->InitCustomFormat(Width, Height, PF_FloatRGBA, false);
        RenderTarget->TargetGamma = 2.2f; 
        // 注意：此时 SRGB 设置对 Float 目标影响不大，主要看 CaptureSource
    }

    // 同样用 FinalColorHDR，跟BMP一致
    SceneCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorHDR;
    SceneCaptureComponent->TextureTarget = RenderTarget;
    SceneCaptureComponent->CaptureScene();

    // 确保 GPU 渲染完成，否则可能读到空数据
    FlushRenderingCommands();

    // 拼接完整文件名，改为 PNG 后缀
    FString FullFilePath;
    if (bOverride)
    {
        FullFilePath = FPaths::Combine(SavePath, FileName + TEXT(".png"));
    }
    else
    {
        int32 FileIndex = LastFileIndex;
        do
        {
            FullFilePath = FPaths::Combine(
                SavePath, 
                FString::Printf(TEXT("%s_%d%d%d%d.png"), *FileName, 0, 0, 0, FileIndex)
            );
            FileIndex++;
        } while (FPaths::FileExists(FullFilePath));
        LastFileIndex = FileIndex;
    }

    if (FullFilePath.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("File path is empty!"));
        return;
    }

    // 读取 Float16 像素（跟BMP版一致）
    FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
    if (!RTResource)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get RenderTargetResource!"));
        return;
    }

    TArray<FFloat16Color> HDRBitmap;
    RTResource->ReadFloat16Pixels(HDRBitmap);
    if (HDRBitmap.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("No HDR pixels read!"));
        return;
    }

    // 和 BMP 代码一致的 Gamma 转换
    // 你的原BMP示例里写了 float Gamma = 0.5f; 这里保留相同值保证效果相同
    float Gamma = 0.5f;  // 你可调成 2.2f 或其它, 跟BMP保持一致就行

    // 转换到 8 位 FColor 数组
    TArray<FColor> LDRBitmap;
    LDRBitmap.Reserve(HDRBitmap.Num());

    for (const FFloat16Color& HDRPixel : HDRBitmap)
    {
        FLinearColor Linear(HDRPixel.R, HDRPixel.G, HDRPixel.B, HDRPixel.A);

        // 应用相同的 Gamma 计算（和你原 BMP 代码一样）
        Linear.R = FMath::Pow(Linear.R, 1.0f / Gamma);
        Linear.G = FMath::Pow(Linear.G, 1.0f / Gamma);
        Linear.B = FMath::Pow(Linear.B, 1.0f / Gamma);

        // clamp到 [0,1]，然后转 FColor
        Linear.R = FMath::Clamp(Linear.R, 0.0f, 1.0f);
        Linear.G = FMath::Clamp(Linear.G, 0.0f, 1.0f);
        Linear.B = FMath::Clamp(Linear.B, 0.0f, 1.0f);
        // Alpha 一般直接 1
        Linear.A = 1.0f;

        LDRBitmap.Add(Linear.ToFColor(true));
    }

    // 后台线程写 PNG，避免卡主线程
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, FullFilePath, LDRBitmap = MoveTemp(LDRBitmap), Debug, Width, Height]()
    {
        IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
        TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

        // 直接声明 BGRA 8 位
        if (ImageWrapper.IsValid() && ImageWrapper->SetRaw(
            LDRBitmap.GetData(),
            LDRBitmap.Num() * sizeof(FColor),
            Width, Height,
            ERGBFormat::BGRA, 8))
        {
            const TArray64<uint8>& PNGData = ImageWrapper->GetCompressed(100);
            if (FFileHelper::SaveArrayToFile(PNGData, *FullFilePath))
            {
                if (Debug)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Saved PNG image to: %s"), *FullFilePath);
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to save PNG to: %s"), *FullFilePath);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to encode PNG image!"));
        }
    });
}






void ASavePhotoPawn::Print(const FString& Target)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::MakeRandomColor(),Target);
	}
}