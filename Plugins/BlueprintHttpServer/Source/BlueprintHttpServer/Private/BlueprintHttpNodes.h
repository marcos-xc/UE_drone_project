// Copyright Pandores Marketplace 2021. All Righst Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintHttpServer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "BlueprintHttpNodes.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_DELEGATE_TwoParams(FHttpServerRouteMulticastCallback, const FBlueprintHttpRequest&, HttpRequest, FBlueprintHttpResponse, HttpResponse);

UENUM()
enum class ESuccessFailBranching : uint8
{
	Done,
	Failed
};

USTRUCT(BlueprintType, meta = (DontUseGenericSpawnObject = "True"))
struct FHttpServerMountFolder
{
	GENERATED_BODY()
public:
	/**
	 * The location on the disk of the files to mount.
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Http|Server")
	FString DiskLocation;

	/**
	 * The target route to reach this folder. Could be "/" or "/someroute/".
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Http|Server")
	FString Route;

	/**
	 * The default additional headers when accessing this route.
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Http|Server")
	TMap<FString, FString> DefaultHeaders;
};

USTRUCT(BlueprintType, meta=(DontUseGenericSpawnObject = "True"))
struct FHttpServerRouteListener
{
	GENERATED_BODY()
public:
	/**
	 * The verb used to access this route.
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Http|Server")
	EHttpServerVerb Verb;

	/**
	 * The target route to execute this listener.
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Http|Server")
	FString Route;

	/**
	 * The callback called when a client request this route.
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Http|Server")
	FHttpServerRouteMulticastCallback Callback;

	/**
	 * If we want to execute the callback on game thread or not.
	 * Not executing it on game thread makes it way faster to respond.
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Http|Server")
	bool bRequireGameThread = false;
};

UCLASS()
class UBlueprintHttpServerLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	/**
	 * Setup multiple routes at the same time for this HTTP(S) server.
	 * @param FoldersToMount The folders to mount as static files.
	 * @param RouteListeners The route listeners to add.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server", meta = (AutoCreateRefTerm = "FoldersToMount, RouteListeners", Keywords = "setup routes listen url path"))
	static void SetupRoutes(UBlueprintHttpServer* HttpServer, const TArray<FHttpServerMountFolder>& FoldersToMount, const TArray<FHttpServerRouteListener>& RouteListeners);

	/**
	 * Adds a path to where we server files. The search is applied
	 * according to calls of this function.
	 * You should call the primary mounting points first.
	 * @param UrlPath  The URL to reach the folder to mount.
	 * @param DiskPath The path on the disk of the folder to mount.
	 * @param DefaultHeaders The default headers added for this point.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server", meta = (AutoCreateRefTerm = "DefaultHeaders", ExpandEnumAsExecs = "Branch", Keywords = "add mount point static path"))
	static void AddMountPoint(UBlueprintHttpServer* HttpServer, const FString& UrlPath, const FString& DiskPath, const TMap<FString, FString>& DefaultHeaders, ESuccessFailBranching& Branch);

	/**
	 * Removes a path to where we serve files.
	 * @param UrlPath  The URL to reach the folder to mount.
	 * @return If the operation succeeded.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server", meta = (ExpandEnumAsExecs = "Branch"))
	static void RemoveMountPoint(UBlueprintHttpServer* HttpServer, const FString& UrlPath, ESuccessFailBranching& Branch);

	/**
	 * Adds a callback called when a client request a specific route.
	 * @param HttpServer The Http Server we want to bind the callback to.
	 * @param Verb The verb of the route.
	 * @param Route The address of the route. (Could be "/" or "/games/").
	 * @param Callback Callback called when a client request the route.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server")
	static void AddRoute(UBlueprintHttpServer* HttpServer, const EHttpServerVerb Verb, const FString& Route, const bool bRequireGameThread, FHttpServerRouteMulticastCallback Callback);


	///////////////////////////////////////////////////////
	// FBlueprintHttpResponse nodes.

	/**
	 * Set the response's body, erasing all previously set body data.
	 * @param Body The body to set.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server|Response")
	static void SetBody(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const FString& Body);

	/**
	 * Set the response's content with a Mime-Type.
	 * @param Content	The response's content.
	 * @param MimeType	The response's Mime-Type.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server|Response")
	static void SetContent(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const FString& Content, const FString& MimeType);

	/**
	 * Set the response's content with a Mime-Type.
	 * @param Content	The response's content.
	 * @param MimeType	The response's Mime-Type.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server|Response")
	static void SetBinaryContent(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const TArray<uint8>& Content, const FString& MimeType);

	/**
	 * Appends a string to the current body content.
	 * @param Body What to append to the body.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server|Response")
	static void AppendToBody(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const FString& Body);

	/**
	 * Empties the previously set response body.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server|Response")
	static void EmptyBody(UPARAM(ref)FBlueprintHttpResponse& HttpResponse);

	/**
	 * Adds a single header.
	 * Prefer AddHeaders() if you add multiple headers.
	 * @param Key   The header's key.
	 * @param Value The header's value.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server|Response")
	static void AddHeader(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const FString& Key, const FString& Value);

	/**
	 * Removes a previously added header.
	 * @param Key The key of the header we want to remove.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server|Response")
	static void RemoveHeader(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const FString& Key);

	/**
	 * Adds multiple headers to the response.
	 * For performance, consider calling this method with several headers entry
	 * than calling AddHeader() several times.
	 * @param Headers The headers to add to the response.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server|Response")
	static void AddHeaders(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const TMap<FString, FString>& Headers);

	/**
	 * Sets the response status.
	 * @param Status The response status.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server|Response")
	static void SetStatus(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const int32 Status);

	/**
	 * Sets the version.
	 * @param Version The version to set.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server|Response")
	static void SetVersion(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const FString& Version);

	/**
	 * Sets a redirect location.
	 * @param Location The location where we want to redirect the client.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server|Response")
	static void SetRedirectLocation(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const FString& Location);

	/**
	 * Sets the response reason.
	 * @param Reason The reason to set.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server|Response")
	static void SetReason(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const FString& Reason);

	/**
	 * Send the response.
	 * After a call to this method, all FBlueprintHttpResponse copied from
	 * this structure and this structure become invalid.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server|Response", meta=(KeyWords="Send End Terminate Respond"))
	static void Send(UPARAM(ref)FBlueprintHttpResponse& HttpResponse);

	///////////////////////////////////////////////
	// BlueprintHttpRequest

	/**
	 * Checks if the request has the specified header.
	 * @param HeaderKey The key of the header.
	 * @return If the specified header key is in the header.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Http|Server|Request")
	static UPARAM(DisplayName = "Has Header") bool HasHeader(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest, const FString& HeaderKey);

	/**
	 * Gets a single header matching the key.
	 * @param HeaderKey The key of the header to get.
	 * @return The header value.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Http|Server|Request")
	static UPARAM(DisplayName = "Header") FString GetHeader(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest, const FString& HeaderKey);

	/**
	 * Gets the all the headers of the request.
	 * @return All headers of the request.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Http|Server|Request")
	static UPARAM(DisplayName = "Headers") TMap<FString, FString> GetHeaders(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest);

	/**
	 * Gets the request's body.
	 * @return The request's body.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Http|Server|Request")
	static UPARAM(DisplayName = "Body") FString GetBody(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest);

	/**
	 * Checks if the URL contains the specified parameter.
	 * @param Name The name of the parameter.
	 * @return True if the URL contains the parameter.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Http|Server|Request")
	static UPARAM(DisplayName = "Has Parameter") bool HasUrlParameter(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest, const FString& Name);

	/**
	 * Gets an URL parameter
	 * @param Name The name of the parameter.
	 * @return The value of the parameter.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Http|Server|Request")
	static UPARAM(DisplayName = "Parameter") FString GetUrlParameter(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest, const FString& Name);

	/**
	 * Get the request's remote address.
	 * @return The request's remote address.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Http|Server|Request")
	static UPARAM(DisplayName = "Address") FString GetRemoteAddress(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest);

	/**
	 * Get the remote's port.
	 * @return The remote's port.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Http|Server|Request")
	static UPARAM(DisplayName = "Port") int32 GetRemotePort(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest);

	/**
	 * Gets the verb for the request.
	 * @return The verb.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Http|Server|Request")
	static UPARAM(DisplayName = "Verb") FString GetVerb(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynMultNoParam);

UCLASS()
class UHttpServerListenProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Called when the server successfully started listening.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultNoParam OnListening;

	/**
	 * Called when the server failed to start listening.
	 * More information is available in the output log.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultNoParam OnFailed;

public:
	virtual void Activate() override;

	/**
	 * Start listening for new client connections.
	 * @param HttpServer The HTTP(S) server that should start listening.
	 * @param Host		 The host where we want to listen to.
	 * @param Port		 The port we want to listen to. 0 for all.
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Http|Server")
	static UHttpServerListenProxy* Listen(UBlueprintHttpServer* const HttpServer, const FString& Host, const int32 Port);

private:
	UFUNCTION()
	void OnActionOver(const bool bSuccess);

private:
	UPROPERTY()
	UBlueprintHttpServer* Server;

	FString Host;
	int32 Port;
};

