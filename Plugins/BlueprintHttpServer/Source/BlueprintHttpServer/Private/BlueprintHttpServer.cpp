// Copyright Pandores Marketplace 2021. All Righst Reserved.

#include "BlueprintHttpServer.h"

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

#include <mutex>
#include <condition_variable>

#include "Async/Async.h"

#include "BlueprintHttpServerModule.h"

#define LAMBDA_MOVE(x) x = MoveTemp(x)

#define START_INTERNAL_SYNCHRONIZED(...) \
	Internal->ExecuteLocked([&](__VA_ARGS__) -> void {
#define END_INTERNAL_SYNCHRONIZED(...)   \
	});

typedef httplib::Server& (httplib::Server::*FRouteSetupListener)(const char*, std::function<void(const httplib::Request&, httplib::Response&)>);

template<class T>
class FBlueprintHttpRequestInternal final
{
private:
	using FInternalType				= T;
	using FConditionVariable		= std::condition_variable;
	using FExecuteLockedFunction	= TUniqueFunction<void(FInternalType&)>;

public:
	FBlueprintHttpRequestInternal(T* const Request)
		: Internal(Request)
		, Waiter(nullptr)
	{}

	bool IsValid()
	{
		FScopeLock Lock(&Mutex);
		return Internal != nullptr;
	}

	void Invalidate()
	{
		{
			FScopeLock Lock(&Mutex);
			Internal = nullptr;
		}
		
		if (Waiter)
		{
			Waiter->notify_all();
			Waiter = nullptr;
		}
	}

	void ExecuteLocked(FExecuteLockedFunction Function)
	{
		FScopeLock Lock(&Mutex);

		if (Internal)
		{
			Function(*Internal);
		}
	}

	void SetupWaiter(FConditionVariable* const InWaiter)
	{
		Waiter = InWaiter;
	}

private:
	FCriticalSection	Mutex;
	FInternalType*		Internal;
	FConditionVariable* Waiter;
};

class FRouteListener
{
private:
	FRouteListener() = delete;
public:
	static void SetupRouteListener(httplib::Server& Server, FRouteSetupListener Listener, const FString& Path, 
		FHttpServerRouteCallback& Callback, const bool bRequireGameThread, const long long MillisecondsToWait)
	{
		(Server.*Listener) (TCHAR_TO_UTF8(*Path), [LAMBDA_MOVE(Callback), bRequireGameThread, MillisecondsToWait]
	
		(const httplib::Request& Req, httplib::Response& Res) -> void
		{
			if (!Callback.IsBound())
			{
				return;
			}

			FBlueprintHttpRequest  Request (&Req);
			FBlueprintHttpResponse Response(&Res);

			const auto DispatchCallback = [&]() -> void
			{
				if (bRequireGameThread)
				{
					AsyncTask(ENamedThreads::GameThread, [Callback, Request, Response]() mutable -> void
					{
						Callback.ExecuteIfBound(Request, Response);
					});
				}
				else
				{
					Callback.ExecuteIfBound(Request, Response);
				}
			};

			// We don't wait, no need for condition_variable or mutex.
			if (FMath::IsNearlyEqual(MillisecondsToWait, 0.f))
			{
				DispatchCallback();
			}

			// We have to wait for completion
			else
			{
				std::mutex				LocalMutex;
				std::condition_variable LocalWaiter;

				Response.Internal->SetupWaiter(&LocalWaiter);

				DispatchCallback();

				// If the response is still valid, we have to wait for it.
				// It basically means Send() hasn't been called and the user
				// is still performing treatment on it.
				if (Response.Internal->IsValid())
				{
					std::unique_lock<std::mutex>	Lock(LocalMutex);

					const auto WaitUntil = std::chrono::steady_clock().now() + std::chrono::milliseconds(MillisecondsToWait);
					const std::cv_status Status = LocalWaiter.wait_until(Lock, WaitUntil);

					if (Status == std::cv_status::timeout)
					{
						UE_LOG(LogHttpServer, Warning, TEXT("Reached timeout of %.3f seconds for HTTP Request."), MillisecondsToWait / 1000.f);
					}
				}
			}

			Response.Internal->Invalidate();
			Request .Internal->Invalidate();
		});
	}
};

void FBlueprintHttpResponse::SetBody(const FString& Body)
{
	START_INTERNAL_SYNCHRONIZED(httplib::Response & Response);
	
	Response.body = TCHAR_TO_UTF8(*Body);
	
	END_INTERNAL_SYNCHRONIZED();
}

void FBlueprintHttpResponse::SetContent(const FString& Content, const FString& MimeType)
{
	START_INTERNAL_SYNCHRONIZED(httplib::Response & Response);

	Response.set_content(TCHAR_TO_UTF8(*Content), TCHAR_TO_UTF8(*MimeType));

	END_INTERNAL_SYNCHRONIZED();
}

void FBlueprintHttpResponse::SetContent(const TArray<uint8>& Content, const FString& MimeType)
{
	START_INTERNAL_SYNCHRONIZED(httplib::Response & Response);

	Response.set_content((const char*)Content.GetData(), Content.Num(), TCHAR_TO_UTF8(*MimeType));

	END_INTERNAL_SYNCHRONIZED();
}

void FBlueprintHttpResponse::AppendToBody(const FString& Body)
{
	START_INTERNAL_SYNCHRONIZED(httplib::Response & Response);

	Response.body += TCHAR_TO_UTF8(*Body);

	END_INTERNAL_SYNCHRONIZED();
}

void FBlueprintHttpResponse::EmptyBody()
{
	START_INTERNAL_SYNCHRONIZED(httplib::Response & Response);

	Response.body.clear();

	END_INTERNAL_SYNCHRONIZED();
}

void FBlueprintHttpResponse::AddHeaders(const TMap<FString, FString>& Headers)
{
	START_INTERNAL_SYNCHRONIZED(httplib::Response & Response);

	for (const auto& Header : Headers)
	{
		Response.headers.insert({ TCHAR_TO_UTF8(*Header.Key), TCHAR_TO_UTF8(*Header.Value) });
	}

	END_INTERNAL_SYNCHRONIZED();
}

void FBlueprintHttpResponse::AddHeader(const FString& Key, const FString& Value)
{
	START_INTERNAL_SYNCHRONIZED(httplib::Response & Response);

	Response.headers.insert({ TCHAR_TO_UTF8(*Key), TCHAR_TO_UTF8(*Value) });

	END_INTERNAL_SYNCHRONIZED();
}

void FBlueprintHttpResponse::RemoveHeader(const FString& Key)
{
	START_INTERNAL_SYNCHRONIZED(httplib::Response & Response);

	Response.headers.erase(TCHAR_TO_UTF8(*Key));

	END_INTERNAL_SYNCHRONIZED();
}

void FBlueprintHttpResponse::SetStatus(const int32 Status)
{
	START_INTERNAL_SYNCHRONIZED(httplib::Response & Response);

	Response.status = Status;

	END_INTERNAL_SYNCHRONIZED();
}

void FBlueprintHttpResponse::SetVersion(const FString& Version)
{
	START_INTERNAL_SYNCHRONIZED(httplib::Response & Response);

	Response.version = TCHAR_TO_UTF8(*Version);

	END_INTERNAL_SYNCHRONIZED();
}

void FBlueprintHttpResponse::SetRedirectLocation(const FString& Location)
{
	START_INTERNAL_SYNCHRONIZED(httplib::Response & Response);

	Response.set_redirect(TCHAR_TO_UTF8(*Location));

	END_INTERNAL_SYNCHRONIZED();
}

void FBlueprintHttpResponse::SetReason(const FString& Reason)
{
	START_INTERNAL_SYNCHRONIZED(httplib::Response & Response);

	Response.reason = TCHAR_TO_UTF8(*Reason);

	END_INTERNAL_SYNCHRONIZED();
}

bool FBlueprintHttpRequest::HasHeader(const FString& HeaderKey) const
{
	bool ReturnValue = false;

	START_INTERNAL_SYNCHRONIZED(const httplib::Request & Request);

	ReturnValue = Request.has_header(TCHAR_TO_UTF8(*HeaderKey));

	END_INTERNAL_SYNCHRONIZED();

	return ReturnValue;
}

FString FBlueprintHttpRequest::GetHeader(const FString& HeaderKey) const
{
	FString ReturnValue;

	START_INTERNAL_SYNCHRONIZED(const httplib::Request & Request);

	ReturnValue = UTF8_TO_TCHAR(Request.get_header_value(TCHAR_TO_UTF8(*HeaderKey)).c_str());

	END_INTERNAL_SYNCHRONIZED();

	return ReturnValue;
}

TMap<FString, FString> FBlueprintHttpRequest::GetHeaders() const
{
	TMap<FString, FString> ReturnValue;

	START_INTERNAL_SYNCHRONIZED(const httplib::Request & Request);

	ReturnValue.Reserve(Request.headers.size());
	for (const auto& Header : Request.headers)
	{
		ReturnValue.Emplace(UTF8_TO_TCHAR(Header.first.c_str()), UTF8_TO_TCHAR(Header.second.c_str()));
	}

	END_INTERNAL_SYNCHRONIZED();

	return ReturnValue;
}

FString FBlueprintHttpRequest::GetBody() const
{
	FString ReturnValue;

	START_INTERNAL_SYNCHRONIZED(const httplib::Request & Request);

	ReturnValue = UTF8_TO_TCHAR(Request.body.c_str());

	END_INTERNAL_SYNCHRONIZED();

	return ReturnValue;
}

FString FBlueprintHttpRequest::GetUrlParameter(const FString& Name) const
{
	FString ReturnValue;

	START_INTERNAL_SYNCHRONIZED(const httplib::Request & Request);

	ReturnValue = UTF8_TO_TCHAR(Request.get_param_value(TCHAR_TO_UTF8(*Name)).c_str());

	END_INTERNAL_SYNCHRONIZED();

	return ReturnValue;
}

bool FBlueprintHttpRequest::HasUrlParameter(const FString& Name) const
{
	bool bReturnValue = false;

	START_INTERNAL_SYNCHRONIZED(const httplib::Request & Request);

	bReturnValue = Request.has_param(TCHAR_TO_UTF8(*Name));

	END_INTERNAL_SYNCHRONIZED();

	return bReturnValue;
}

FString FBlueprintHttpRequest::GetRemoteAddress() const
{
	FString ReturnValue;

	START_INTERNAL_SYNCHRONIZED(const httplib::Request & Request);

	ReturnValue = UTF8_TO_TCHAR(Request.remote_addr.c_str());
	
	END_INTERNAL_SYNCHRONIZED();

	return ReturnValue;
}

int32 FBlueprintHttpRequest::GetRemotePort() const
{
	int32 ReturnValue = -1;

	START_INTERNAL_SYNCHRONIZED(const httplib::Request & Request);

	ReturnValue = Request.remote_port;

	END_INTERNAL_SYNCHRONIZED();

	return ReturnValue;
}

FString FBlueprintHttpRequest::GetVerb() const
{
	FString ReturnValue;

	START_INTERNAL_SYNCHRONIZED(const httplib::Request & Request);

	ReturnValue = UTF8_TO_TCHAR(Request.method.c_str());

	END_INTERNAL_SYNCHRONIZED();

	return ReturnValue;
}

void FBlueprintHttpResponse::Send()
{
	Internal->Invalidate();
}

void FBlueprintHttpResponse::End()
{
	Send();
}

FBlueprintHttpResponse::FBlueprintHttpResponse(httplib::Response* const InternalResponse)
	: Internal(MakeShared<FBlueprintHttpRequestInternal<httplib::Response>, ESPMode::ThreadSafe>(InternalResponse))
{
}

FBlueprintHttpRequest::FBlueprintHttpRequest(const httplib::Request* const	InternalRequest)
	: Internal(MakeShared<FBlueprintHttpRequestInternal<const httplib::Request>, ESPMode::ThreadSafe>(InternalRequest))
{	
}

FBlueprintHttpResponse::FBlueprintHttpResponse(FBlueprintHttpResponse&& Other)
	: Internal(MoveTemp(Other.Internal))
{
}

FBlueprintHttpRequest::FBlueprintHttpRequest(const FBlueprintHttpRequest& Other)
	: Internal(Other.Internal)
{
}

FBlueprintHttpRequest::FBlueprintHttpRequest(FBlueprintHttpRequest&& Other)
	: Internal(MoveTemp(Other.Internal))
{
}

FBlueprintHttpResponse::FBlueprintHttpResponse(const FBlueprintHttpResponse& Other)
	: Internal(Other.Internal)
{
}

FBlueprintHttpRequest& FBlueprintHttpRequest::operator=(const FBlueprintHttpRequest& Other)
{
	Internal = Other.Internal;
	return *this;
}

FBlueprintHttpRequest& FBlueprintHttpRequest::operator=(FBlueprintHttpRequest&& Other)
{
	Internal = MoveTemp(Other.Internal);
	return *this;
}

FBlueprintHttpResponse& FBlueprintHttpResponse::operator=(const FBlueprintHttpResponse& Other)
{
	Internal = Other.Internal;
	return *this;
}

FBlueprintHttpResponse& FBlueprintHttpResponse::operator=(FBlueprintHttpResponse&& Other)
{
	Internal = MoveTemp(Other.Internal);
	return *this;
}

UBlueprintHttpServer::UBlueprintHttpServer(FVTableHelper& Helper) 
	: UBlueprintHttpServer()
{
}

static long long GetMillisecondsTimeout(const float& SecondsTimeout)
{
	return static_cast<long long>(SecondsTimeout * 1000.f);
}

UBlueprintHttpServer::UBlueprintHttpServer()
	: Super()
	, Server(MakeShared<httplib::Server, ESPMode::ThreadSafe>())
	, MaxSecondWaitTimeout(5.f)
{
}

UBlueprintHttpServer::~UBlueprintHttpServer()
{
	if (Server->is_running())
	{
		UE_LOG(LogHttpServer, Warning, TEXT("BlueprintHttpServer wasn't stopped but it got garbage collected. Forcing server to stop."));
		
		Stop();
	}

	UE_LOG(LogHttpServer, Log, TEXT("BlueprintHttpServer destroyed."));
}

UBlueprintHttpServer* UBlueprintHttpServer::CreateHttpServer()
{
	UBlueprintHttpServer* const Server = NewObject<UBlueprintHttpServer>();

	return Server;
}

void UBlueprintHttpServer::Stop()
{
	Server->stop();	

	UE_LOG(LogHttpServer, Log, TEXT("BlueprintHttpServer stopped."));
}

bool UBlueprintHttpServer::IsRunning() const
{
	return Server->is_running();
}

UBlueprintHttpServer* UBlueprintHttpServer::Get(const FString& Path, FHttpServerRouteCallback Callback, const bool bRequireGameThread)
{
	UE_LOG(LogHttpServer, Log, TEXT("New route added: { GET, %s }."), *Path);
	FRouteListener::SetupRouteListener(*Server, &httplib::Server::Get, Path, Callback, bRequireGameThread, MaxSecondWaitTimeout * 1000);
	return this;
}

UBlueprintHttpServer* UBlueprintHttpServer::Post(const FString& Path, FHttpServerRouteCallback Callback, const bool bRequireGameThread)
{
	UE_LOG(LogHttpServer, Log, TEXT("New route added: { POST, %s }."), *Path);
	FRouteListener::SetupRouteListener(*Server, &httplib::Server::Post, Path, Callback, bRequireGameThread, GetMillisecondsTimeout(MaxSecondWaitTimeout));
	return this;
}

UBlueprintHttpServer* UBlueprintHttpServer::Patch(const FString& Path, FHttpServerRouteCallback Callback, const bool bRequireGameThread)
{
	UE_LOG(LogHttpServer, Log, TEXT("New route added: { PATCH, %s }."), *Path);
	FRouteListener::SetupRouteListener(*Server, &httplib::Server::Patch, Path, Callback, bRequireGameThread, GetMillisecondsTimeout(MaxSecondWaitTimeout));
	return this;
}

UBlueprintHttpServer* UBlueprintHttpServer::Delete(const FString& Path, FHttpServerRouteCallback Callback, const bool bRequireGameThread)
{
	UE_LOG(LogHttpServer, Log, TEXT("New route added: { DELETE, %s }."), *Path);
	FRouteListener::SetupRouteListener(*Server, &httplib::Server::Delete, Path, Callback, bRequireGameThread, GetMillisecondsTimeout(MaxSecondWaitTimeout));
	return this;
}

UBlueprintHttpServer* UBlueprintHttpServer::Options(const FString& Path, FHttpServerRouteCallback Callback, const bool bRequireGameThread)
{
	UE_LOG(LogHttpServer, Log, TEXT("New route added: { OPTIONS, %s }."), *Path);
	FRouteListener::SetupRouteListener(*Server, &httplib::Server::Options, Path, Callback, bRequireGameThread, GetMillisecondsTimeout(MaxSecondWaitTimeout));
	return this;
}

UBlueprintHttpServer* UBlueprintHttpServer::Put(const FString& Path, FHttpServerRouteCallback Callback, const bool bRequireGameThread)
{
	UE_LOG(LogHttpServer, Log, TEXT("New route added: { PUT, %s }."), *Path);
	FRouteListener::SetupRouteListener(*Server, &httplib::Server::Put, Path, Callback, bRequireGameThread, GetMillisecondsTimeout(MaxSecondWaitTimeout));
	return this;
}

bool UBlueprintHttpServer::AddMountPoint(const FString& UrlPath, const FString& DiskPath, const TMap<FString, FString>& DefaultHeaders)
{
	if (IsRunning())
	{
		UE_LOG(LogHttpServer, Warning, TEXT("Mount points can't be added when the server is running."));
		return false;
	}

	httplib::Headers Headers;

	for (const auto& Header : DefaultHeaders)
	{
		Headers.insert({ TCHAR_TO_UTF8(*Header.Key), TCHAR_TO_UTF8(*Header.Value) });
	}

	return Server->set_mount_point(TCHAR_TO_UTF8(*UrlPath), TCHAR_TO_UTF8(*DiskPath), MoveTemp(Headers));
}

bool UBlueprintHttpServer::RemoveMountPoint(const FString& Path)
{
	if (IsRunning())
	{
		UE_LOG(LogHttpServer, Warning, TEXT("Mount points can't be removed when the server is running."));
		return false;
	}

	return Server->remove_mount_point(TCHAR_TO_UTF8(*Path));
}

void UBlueprintHttpServer::Listen(FString Host, const uint16 Port, FHttpServerListenCallback Callback)
{
	if (IsRunning())
	{
		UE_LOG(LogHttpServer, Warning, TEXT("Called `Listen` but the Http Server is already running."));
		Callback.ExecuteIfBound(false);
		return;
	}

	if (!Server->is_valid())
	{
		UE_LOG(LogHttpServer, Error, TEXT("Called `Listen()` on an invalid server."));
		Callback.ExecuteIfBound(false);
		return;
	}

	Thread.Reset(new std::thread([Server = this->Server, LAMBDA_MOVE(Host), Port, LAMBDA_MOVE(Callback)]() mutable -> void
	{
		const bool bResult = Server->bind_to_port(TCHAR_TO_UTF8(*Host), Port);

		if (bResult)
		{
			UE_LOG(LogHttpServer, Log, TEXT("Started listening on %s:%d."), *Host, Port);
		}
		else
		{
#if PLATFORM_WINDOWS
			UE_LOG(LogHttpServer, Error, TEXT("Failed to bind server to %s:%d. WSALastError: %s"), *Host, Port, *GetWinSockError());
#else
			UE_LOG(LogHttpServer, Error, TEXT("Failed to bind server to %s:%d. %s"), *Host, Port);
#endif
		}

		if (Callback.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [bResult, LAMBDA_MOVE(Callback)]() -> void
			{
				Callback.ExecuteIfBound(bResult);
			});
		}

		const bool bBindSucceeded = Server->listen_after_bind();

		ensure(bBindSucceeded);
	}));

	Thread->detach();
}

void UBlueprintHttpServer::SetFileExtensionAndMimeTypeMapping(const FString& Extension, const FString& MimeType)
{
	if (IsRunning())
	{
		UE_LOG(LogHttpServer, Warning, TEXT("Mount points can't be removed when the server is running."));
		return;
	}

	Server->set_file_extension_and_mimetype_mapping(TCHAR_TO_UTF8(*Extension), TCHAR_TO_UTF8(*MimeType));
}

void UBlueprintHttpServer::SetKeepAliveMaxCount(const int32 Count)
{
	ensure(Count > 0);

	Server->set_keep_alive_max_count((size_t)Count);
}

void UBlueprintHttpServer::SetKeepAliveTimeout(const int32 Timeout)
{
	ensure(Timeout >= 0);

	Server->set_keep_alive_timeout((time_t)Timeout);
}

void UBlueprintHttpServer::SetPayloadMaxLength(const int32 MaxLength)
{
	ensure(MaxLength > 0);
	
	Server->set_payload_max_length(MaxLength);
}

void UBlueprintHttpServer::SetTcpNodelay(const bool bTcpNodelay)
{
	Server->set_tcp_nodelay(bTcpNodelay);
}

void UBlueprintHttpServer::SetHttpThreadPoolSize(const int32 ThreadPoolSize)
{
	Server->new_task_queue = [ThreadPoolSize]() -> httplib::ThreadPool*
	{
		return new httplib::ThreadPool(ThreadPoolSize);
	};
}

void UBlueprintHttpServer::SetMaxWaitingDelayForResponse(const float InSecondsToWait)
{
	ensure(InSecondsToWait >= 0.f);

	MaxSecondWaitTimeout = InSecondsToWait;
}

