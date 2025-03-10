// Copyright Pandores Marketplace 2021. All Righst Reserved.

#include "BlueprintHttpsServer.h"

#if PLATFORM_WINDOWS
#	include "Windows/AllowWindowsPlatformTypes.h"
#endif // PLATFORM_WINDOWS

#define UI UI_ST
THIRD_PARTY_INCLUDES_START
#	include <httplib.h>
THIRD_PARTY_INCLUDES_END
#undef UI

#if PLATFORM_WINDOWS
#	include "Windows/HideWindowsPlatformTypes.h"
#endif // PLATFORM_WINDOWS

#include "Misc/Paths.h"

#include "BlueprintHttpServerModule.h"

static int SslErrorCallback(const char* str, size_t len, void* const)
{
	if (len > 0)
	{
		FString Error(str, len);

		UE_LOG(LogHttpServer, Error, TEXT("OpenSSL error: %s"), *Error);
	}

	return 0;
}

UBlueprintHttpsServer* UBlueprintHttpsServer::CreateHttpsServer(const FString& CertPath, const FString& PrivateKeyPath,
	const FString& ClientCaCertFilePath, const FString& ClientCaCertDirPath)
{
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT

	if (!FPaths::FileExists(CertPath))
	{
		UE_LOG(LogHttpServer, Error, TEXT("Certificate with path \"%s\" doesn't exist."), *CertPath);
		return nullptr;
	}

	if (!FPaths::FileExists(PrivateKeyPath))
	{
		UE_LOG(LogHttpServer, Error, TEXT("Private key with path \"%s\" doesn't exist."), *PrivateKeyPath);
		return nullptr;
	}

	if (!ClientCaCertDirPath.IsEmpty() && !FPaths::DirectoryExists(PrivateKeyPath))
	{
		UE_LOG(LogHttpServer, Error, TEXT("Client CA certificate directory with path \"%s\" doesn't exist."), *PrivateKeyPath);
		return nullptr;
	}

	if (!ClientCaCertFilePath.IsEmpty() && !FPaths::FileExists(ClientCaCertFilePath))
	{
		UE_LOG(LogHttpServer, Error, TEXT("Client CA certificate with path \"%s\" doesn't exist."), *PrivateKeyPath);
		return nullptr;
	}

	UBlueprintHttpsServer* const Server = NewObject<UBlueprintHttpsServer>();

	int32 Error = 0;

	Server->Server = MakeShared<httplib::SSLServer, ESPMode::ThreadSafe>
	(
		TCHAR_TO_UTF8(*CertPath),
		TCHAR_TO_UTF8(*PrivateKeyPath),
		ClientCaCertFilePath.IsEmpty() ? nullptr : TCHAR_TO_UTF8(*ClientCaCertFilePath),
		ClientCaCertDirPath .IsEmpty() ? nullptr : TCHAR_TO_UTF8(*ClientCaCertDirPath),
		&Error
	);

	httplib::SSLServer* const SslServer = (httplib::SSLServer*)Server->Server.Get();

	if (!SslServer->is_valid())
	{
		if (!SslServer->ctx_)
		{
			ERR_print_errors_cb(&SslErrorCallback, nullptr);
		}

		switch (Error)
		{
		case 1: UE_LOG(LogHttpServer, Error, TEXT("HTTPS server creation error: failed to create SSL_CTX.")); break;
		case 2: UE_LOG(LogHttpServer, Error, TEXT("HTTPS server creation error: invalid certificate."));	  break;
		case 3: UE_LOG(LogHttpServer, Error, TEXT("HTTPS server creation error: invalid private key."));	  break;
		}

		return nullptr;
	}

	return Server;

#else // !CPPHTTPLIB_OPENSSL_SUPPORT

	UE_LOG(LogHttpServer, Warning, TEXT("HTTPS server is not supported for this platform. The server will be an HTTP server instead."));

	return NewObject<UBlueprintHttpsServer>();

#endif // !CPPHTTPLIB_OPENSSL_SUPPORT
}

UBlueprintHttpsServer::UBlueprintHttpsServer() : UBlueprintHttpServer()
{
}
