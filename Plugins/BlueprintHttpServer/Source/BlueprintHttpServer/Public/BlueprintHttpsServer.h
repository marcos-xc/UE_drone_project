// Copyright Pandores Marketplace 2021. All Righst Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintHttpServer.h"
#include "BlueprintHttpsServer.generated.h"

/**
 * An HTTPS server.
*/
UCLASS(BlueprintType, meta = (DontUseGenericSpawnObject = "True"))
class BLUEPRINTHTTPSERVER_API UBlueprintHttpsServer : public UBlueprintHttpServer
{
	GENERATED_BODY()
public:
	// Don't use. Use CreateHttpsServer() instead.
	UBlueprintHttpsServer();

	/**
	 * Creates a new HTTPS server.
	 * @param CertPath				The server's certificate path.
	 * @param PrivateKeyPath		The server's private key path.
	 * @param ClientCaCertFilePath	(Optional) The client's CA file path.
	 * @param ClientCaCertDirPath	(Optional) The client's CA certificate directory.
	 * @return A new HTTPS server.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Http|Server", meta=(DisplayName = "Create HTTPS Server"))
	static UPARAM(DisplayName = "HTTPS Server") UBlueprintHttpsServer* CreateHttpsServer(const FString& CertPath, const FString& PrivateKeyPath, 
		const FString& ClientCaCertFilePath = TEXT(""), const FString& ClientCaCertDirPath = TEXT(""));
};
