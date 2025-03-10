// Copyright Pandores Marketplace 2021. All Righst Reserved.

#pragma once

#include <thread>

#include "CoreMinimal.h"
#include "BlueprintHttpServer.generated.h"

// Forward declaration of internal types.
namespace httplib { class Server; struct Request; struct Response; }
template<class T> class FBlueprintHttpRequestInternal;

/**
 * An HTTP verb.
*/
UENUM(BlueprintType)
enum class EHttpServerVerb : uint8
{
	Get,
	Post,
	Delete,
	Options,
	Patch,
	Put,

	MAX UMETA(Hidden)
};

/**
 * Delegate called when a route is requested by a client.
 * @param Request The request object sent by the client.
 * @param Response The response object we will send to the client.
*/
DECLARE_DELEGATE_TwoParams(FHttpServerRouteCallback, const FBlueprintHttpRequest& /* Request */, FBlueprintHttpResponse& /* Response */);

/**
 * Delegate called once the server successfully started to listen
 * for new client connections.
 * @param bSuccess If we successfully started to listen.
*/
DECLARE_DELEGATE_OneParam(FHttpServerListenCallback, const bool /* bSuccess */);

/**
 * An HTTP(S) request.
 **/
USTRUCT(BlueprintType)
struct BLUEPRINTHTTPSERVER_API FBlueprintHttpRequest
{
	GENERATED_BODY()
private:
	friend class FRouteListener;

private:
	// Constructs a valid HttpResponse
	FBlueprintHttpRequest(const httplib::Request* const	InternalRequest);

public:
	// Constructs an invalid HttpRequest.
	FBlueprintHttpRequest() = default;

	// Copy constructor, you can safely copy an HttpRequests.
	FBlueprintHttpRequest(const FBlueprintHttpRequest&);

	// Move constructor, you can safely move an HttpRequests.
	FBlueprintHttpRequest(FBlueprintHttpRequest&&);

	// Assign operators, you can safely move an HttpRequest.
	FBlueprintHttpRequest& operator=(const FBlueprintHttpRequest&);
	FBlueprintHttpRequest& operator=(FBlueprintHttpRequest&&);

	/**
	 * Checks if the request has the specified header.
	 * @param HeaderKey The key of the header.
	 * @return If the specified header key is in the header.
	*/
	bool HasHeader(const FString& HeaderKey) const;

	/**
	 * Gets a single header matching the key.
	 * @param HeaderKey The key of the header to get.
	 * @return The header value.
	*/
	FString GetHeader(const FString& HeaderKey) const;

	/**
	 * Gets the all the headers of the request.
	 * @return All headers of the request.
	*/
	TMap<FString, FString> GetHeaders() const;

	/**
	 * Gets the request's body.
	 * @return The request's body.
	*/
	FString GetBody() const;

	/**
	 * Checks if the URL contains the specified parameter.
	 * @param Name The name of the parameter.
	 * @return True if the URL contains the parameter.
	*/
	bool HasUrlParameter(const FString& Name) const;

	/**
	 * Gets an URL parameter
	 * @param Name The name of the parameter.
	 * @return The value of the parameter.
	*/
	FString GetUrlParameter(const FString& Name) const;

	/**
	 * Get the request's remote address.
	 * @return The request's remote address.
	*/
	FString GetRemoteAddress() const;

	/**
	 * Get the remote's port.
	 * @return The remote's port.
	*/
	int32 GetRemotePort() const;

	/**
	 * Gets the verb for the request.
	 * @return The verb.
	*/
	FString GetVerb() const;

private:
	TSharedPtr<FBlueprintHttpRequestInternal<const httplib::Request>, ESPMode::ThreadSafe> Internal;
};

/**
 * An HTTP(S) response.
 **/
USTRUCT(BlueprintType)
struct BLUEPRINTHTTPSERVER_API FBlueprintHttpResponse
{
	GENERATED_BODY()
private:
	friend class FRouteListener;

private:
	// Constructs a valid HttpResponse
	FBlueprintHttpResponse(httplib::Response* const InternalResponse);

public:
	// Constructs an invalid HttpResponse.
	FBlueprintHttpResponse() = default;

	// Copy constructor, you can safely copy an HttpResponse.
	FBlueprintHttpResponse(const FBlueprintHttpResponse&);

	// Move constructor, you can safely move an HttpResponse.
	FBlueprintHttpResponse(FBlueprintHttpResponse&&);

	// Assign operators. HttpResponses can be safely assigned.
	FBlueprintHttpResponse& operator=(const FBlueprintHttpResponse&);
	FBlueprintHttpResponse& operator=(FBlueprintHttpResponse&&);

	/**
	 * Set the response's body, erasing all previously set body data.
	 * @param Body The body to set.
	*/
	void SetBody(const FString& Body);

	/**
	 * Set the response's content with a Mime-Type.
	 * @param Content	The response's content.
	 * @param MimeType	The response's Mime-Type.
	*/
	void SetContent(const FString& Content, const FString& MimeType);
	void SetContent(const TArray<uint8>& Content, const FString& MimeType);

	/**
	 * Appends a string to the current body content.
	 * @param Body What to append to the body.
	*/
	void AppendToBody(const FString& Body);

	/**
	 * Empties the previously set response body.
	*/
	void EmptyBody();

	/**
	 * Adds a single header.
	 * Prefer AddHeaders() if you add multiple headers.
	 * @param Key   The header's key.
	 * @param Value The header's value.
	*/
	void AddHeader(const FString& Key, const FString& Value);

	/**
	 * Removes a previously added header.
	 * @param Key The key of the header we want to remove.
	*/
	void RemoveHeader(const FString& Key);

	/**
	 * Adds multiple headers to the response.
	 * For performance, consider calling this method with several headers entry
	 * than calling AddHeader() several times.
	 * @param Headers The headers to add to the response.
	*/
	void AddHeaders(const TMap<FString, FString>& Headers);

	/**
	 * Sets the response status.
	 * @param Status The response status.
	*/
	void SetStatus(const int32 Status);

	/**
	 * Sets the version.
	 * @param Version The version to set.
	*/
	void SetVersion(const FString& Version);

	/**
	 * Sets a redirect location.
	 * @param Location The location where we want to redirect the client.
	*/
	void SetRedirectLocation(const FString& Location);
	
	/**
	 * Sets the response reason.
	 * @param Reason The reason to set.
	*/
	void SetReason(const FString& Reason);

	/**
	 * Send the response.
	 * After a call to this method, all FBlueprintHttpResponse copied from
	 * this structure and this structure become invalid.
	*/
	void Send();

	/**
	 * Exactly the same as Send(). Just another name.
	*/
	void End();

private:
	TSharedPtr<FBlueprintHttpRequestInternal<httplib::Response>, ESPMode::ThreadSafe> Internal;
};

/**
 *	An HTTP Server.
 **/
UCLASS(BlueprintType)
class BLUEPRINTHTTPSERVER_API UBlueprintHttpServer : public UObject
{
	GENERATED_BODY()
private:
	using FHttpServerPtr = TSharedPtr<httplib::Server, ESPMode::ThreadSafe>;
	using FThreadPtr	 = TUniquePtr<std::thread>;

public:

	// Default constructor.
	UBlueprintHttpServer();

	// Internal constructor to support forward declared unique_pointer.
	UBlueprintHttpServer(FVTableHelper& Helper);

	// Default destructor.
	~UBlueprintHttpServer();

	/**
	 * Creates a new HTTP server.
	 * @return A new HTTP server.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Http|Server", meta = (DisplayName = "Create HTTP Server"))
	static UPARAM(DisplayName = "HTTP Server") UBlueprintHttpServer* CreateHttpServer();

	/**
	 * Stop the HTTP server.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server")
	void Stop();

	/**
	 * Checks if the server is running.
	 * @return True if the server is running.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Http|Server")
	UPARAM(DisplayName = "Is Running") bool IsRunning() const;

	/**
	 * Adds a GET listener on the specified path.
	 * @param Path The route path. Could be "/" or "/somepath/".
	 * @param Callback Called when a client request the route.
	 * @param bRequireGameThread If we should run the callback on game thread.
	 * @return The server to chain methods call.
	*/
	UBlueprintHttpServer* Get    (const FString& Path, FHttpServerRouteCallback Callback, const bool bRequireGameThread = false);

	/**
	 * Adds a PUT listener on the specified path.
	 * @param Path The route path. Could be "/" or "/somepath/".
	 * @param Callback Called when a client request the route.
	 * @param bRequireGameThread If we should run the callback on game thread.
	 * @return The server to chain methods call.
	*/
	UBlueprintHttpServer* Put	 (const FString& Path, FHttpServerRouteCallback Callback, const bool bRequireGameThread = false);

	/**
	 * Adds a POST listener on the specified path.
	 * @param Path The route path. Could be "/" or "/somepath/".
	 * @param Callback Called when a client request the route.
	 * @param bRequireGameThread If we should run the callback on game thread.
	 * @return The server to chain methods call.
	*/
	UBlueprintHttpServer* Post   (const FString& Path, FHttpServerRouteCallback Callback, const bool bRequireGameThread = false);

	/**
	 * Adds a PATCH listener on the specified path.
	 * @param Path The route path. Could be "/" or "/somepath/".
	 * @param Callback Called when a client request the route.
	 * @param bRequireGameThread If we should run the callback on game thread.
	 * @return The server to chain methods call.
	*/
	UBlueprintHttpServer* Patch  (const FString& Path, FHttpServerRouteCallback Callback, const bool bRequireGameThread = false);

	/**
	 * Adds a DELETE listener on the specified path.
	 * @param Path The route path. Could be "/" or "/somepath/".
	 * @param Callback Called when a client request the route.
	 * @param bRequireGameThread If we should run the callback on game thread.
	 * @return The server to chain methods call.
	*/
	UBlueprintHttpServer* Delete (const FString& Path, FHttpServerRouteCallback Callback, const bool bRequireGameThread = false);

	/**
	 * Adds a OPTIONS listener on the specified path.
	 * @param Path The route path. Could be "/" or "/somepath/".
	 * @param Callback Called when a client request the route.
	 * @param bRequireGameThread If we should run the callback on game thread.
	 * @return The server to chain methods call.
	*/
	UBlueprintHttpServer* Options(const FString& Path, FHttpServerRouteCallback Callback, const bool bRequireGameThread = false);

	/**
	 * Adds a path to where we serve files. The search is applied
	 * according to calls of this function.
	 * You should call the primary mounting points first.
	 * @param UrlPath  The URL to reach the folder to mount.
	 * @param DiskPath The path on the disk of the folder to mount.
	 * @param DefaultHeaders The default headers added for this point.
	 * @return If the operation succeeded.
	*/
	bool AddMountPoint(const FString& UrlPath, const FString& DiskPath, const TMap<FString, FString>& DefaultHeaders = TMap<FString, FString>());

	/**
	 * Removes a path to where we serve files.
	 * @param UrlPath  The URL to reach the folder to mount.
	 * @return If the operation succeeded.
	*/
	bool RemoveMountPoint(const FString& UrlPath);

	/**
	 * Sets a default Mime-Type match for a file extension when serving static files.
	 * @param Extension The extension we want to change the Mime-Type.
	 * @param MimeType  The Mime-Type associated with the extension.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server")
	void SetFileExtensionAndMimeTypeMapping(const FString& Extension, const FString& MimeType);

	/**
	 * Start the server. After calling this method, your server is ready
	 * to serve the routes and the directories mounted previously.
	 * @param Host The host where to mount the server.
	 * @param Port The port where to mount the server.
	 * @param Callback Called when the server has been mounted.
	*/
	void Listen(FString Host, const uint16 Port, FHttpServerListenCallback Callback = FHttpServerListenCallback());

	/**
	 * The maximum amount of connections to keep alive at the same time.
	 * @param Count The max count of keep alive.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server")
	void SetKeepAliveMaxCount(const int32 Count);

	/**
	 * Sets the keep alive timeout.
	 * @param Timeout The keep alive timeout.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server")
	void SetKeepAliveTimeout(const int32 Timeout);

	/**
	 * Sets the maximum payload length.
	 * @param MaxLength The maximum payload length.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server")
	void SetPayloadMaxLength(const int32 MaxLength);

	/**
	 * Sets it to true if you want to disable Nagle's algorithm.
	 * @param bTcpNodelay If you should use tcp-nodelay.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server")
	void SetTcpNodelay(const bool bTcpNodelay);

	/**
	 * Set the thread pool size used by the HTTP(S) server.
	 * @param ThreadPoolSize The thread pool size.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server")
	void SetHttpThreadPoolSize(const int32 ThreadPoolSize);

	/**
	 * The maximum seconds we wait before sending a response to a client
	 * if the Send() method of the response hasn't been called.
	*/
	UFUNCTION(BlueprintCallable, Category = "Http|Server")
	void SetMaxWaitingDelayForResponse(const float SecondsToWait);

protected:
	/**
	 * Raw Http server pointer.
	*/
	FHttpServerPtr Server;

private:
	/**
	 * The server thread.
	*/
	FThreadPtr Thread;

	/**
	 * The max seconds we wait on a response before
	 * sending it.
	*/
	float MaxSecondWaitTimeout;
};

