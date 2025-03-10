// Copyright Pandores Marketplace 2021. All Righst Reserved.

#include "BlueprintHttpNodes.h"

typedef UBlueprintHttpServer* (UBlueprintHttpServer::* FHttpRouteSetter)(const FString&, FHttpServerRouteCallback, const bool);

static FHttpRouteSetter GetSetterForVerb(const EHttpServerVerb& Verb)
{
	switch (Verb)
	{
	case EHttpServerVerb::Delete:	return &UBlueprintHttpServer::Delete;
	case EHttpServerVerb::Options:	return &UBlueprintHttpServer::Options;
	case EHttpServerVerb::Get:		return &UBlueprintHttpServer::Get;
	case EHttpServerVerb::Post:		return &UBlueprintHttpServer::Post;
	case EHttpServerVerb::Put:		return &UBlueprintHttpServer::Put;
	case EHttpServerVerb::Patch:	return &UBlueprintHttpServer::Patch;
	}

	static_assert((uint8)EHttpServerVerb::MAX == 6u, "A verb is missing");

	return nullptr;
}

void UBlueprintHttpServerLibrary::AddMountPoint(UBlueprintHttpServer* HttpServer, const FString& UrlPath, const FString& DiskPath, const TMap<FString, FString>& DefaultHeaders, ESuccessFailBranching& Branch)
{
	Branch =
		HttpServer && HttpServer->AddMountPoint(UrlPath, DiskPath, DefaultHeaders) ?
		ESuccessFailBranching::Done : ESuccessFailBranching::Failed;
}

void UBlueprintHttpServerLibrary::RemoveMountPoint(UBlueprintHttpServer* HttpServer, const FString& UrlPath, ESuccessFailBranching& Branch)
{
	Branch =
		HttpServer && HttpServer->RemoveMountPoint(UrlPath) ?
		ESuccessFailBranching::Done : ESuccessFailBranching::Failed;
}

void UBlueprintHttpServerLibrary::AddRoute(UBlueprintHttpServer* HttpServer, const EHttpServerVerb Verb, const FString& Route, const bool bRequireGameThread, FHttpServerRouteMulticastCallback Callback)
{
	if (!HttpServer)
	{
		FFrame::KismetExecutionMessage(TEXT("Called AddRoute with an invalid HttpServer pointer."), ELogVerbosity::Error);
		return;
	}

	if (!Callback.IsBound())
	{
		FFrame::KismetExecutionMessage(TEXT("Called AddRoute with an unbound callback."), ELogVerbosity::Warning);
		return;
	}

	const FHttpRouteSetter RouteVerb = GetSetterForVerb(Verb);

	check(RouteVerb != nullptr);

	(HttpServer->*RouteVerb)(Route, FHttpServerRouteCallback::CreateLambda(
		[Callback = MoveTemp(Callback)](const FBlueprintHttpRequest& Request, FBlueprintHttpResponse& Response) -> void
	{
		Callback.ExecuteIfBound(Request, Response);
	}), bRequireGameThread);
}

void UBlueprintHttpServerLibrary::SetupRoutes(UBlueprintHttpServer* HttpServer, const TArray<FHttpServerMountFolder>& FoldersToMount, const TArray<FHttpServerRouteListener>& RouteListeners)
{
	if (!HttpServer)
	{
		FFrame::KismetExecutionMessage(TEXT("Called SetupRoutes with an invalid HttpServer pointer."), ELogVerbosity::Error);
		return;
	}

	for (const auto& FolderToMount : FoldersToMount)
	{
		HttpServer->AddMountPoint(FolderToMount.Route, FolderToMount.DiskLocation, FolderToMount.DefaultHeaders);
	}

	for (const auto& RouteListener : RouteListeners)
	{
		AddRoute(HttpServer, RouteListener.Verb, RouteListener.Route, RouteListener.bRequireGameThread, RouteListener.Callback);
	}
}

void UBlueprintHttpServerLibrary::SetBody(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const FString& Body)
{
	HttpResponse.SetBody(Body);
}

void UBlueprintHttpServerLibrary::AppendToBody(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const FString& Body)
{
	HttpResponse.AppendToBody(Body);
}

void UBlueprintHttpServerLibrary::EmptyBody(UPARAM(ref)FBlueprintHttpResponse& HttpResponse)
{
	HttpResponse.EmptyBody();
}

void UBlueprintHttpServerLibrary::SetContent(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const FString& Content, const FString& MimeType)
{
	HttpResponse.SetContent(Content, MimeType);
}

void UBlueprintHttpServerLibrary::SetBinaryContent(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const TArray<uint8>& Content, const FString& MimeType)
{
	HttpResponse.SetContent(Content, MimeType);
}

void UBlueprintHttpServerLibrary::AddHeader(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const FString& Key, const FString& Value)
{
	HttpResponse.AddHeader(Key, Value);
}

void UBlueprintHttpServerLibrary::RemoveHeader(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const FString& Key)
{
	HttpResponse.RemoveHeader(Key);
}

void UBlueprintHttpServerLibrary::AddHeaders(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const TMap<FString, FString>& Headers)
{
	HttpResponse.AddHeaders(Headers);
}

void UBlueprintHttpServerLibrary::SetStatus(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const int32 Status)
{
	HttpResponse.SetStatus(Status);
}

void UBlueprintHttpServerLibrary::SetVersion(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const FString& Version)
{
	HttpResponse.SetVersion(Version);
}

void UBlueprintHttpServerLibrary::SetRedirectLocation(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const FString& Location)
{
	HttpResponse.SetRedirectLocation(Location);
}

void UBlueprintHttpServerLibrary::SetReason(UPARAM(ref)FBlueprintHttpResponse& HttpResponse, const FString& Reason)
{
	HttpResponse.SetReason(Reason);
}

void UBlueprintHttpServerLibrary::Send(UPARAM(ref)FBlueprintHttpResponse& HttpResponse)
{
	HttpResponse.Send();
}

bool	UBlueprintHttpServerLibrary::HasHeader(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest, const FString& HeaderKey)
{
	return HttpRequest.HasHeader(HeaderKey);
}

FString UBlueprintHttpServerLibrary::GetHeader(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest, const FString& HeaderKey)
{
	return HttpRequest.GetHeader(HeaderKey);
}

TMap<FString, FString> UBlueprintHttpServerLibrary::GetHeaders(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest)
{
	return HttpRequest.GetHeaders();
}

FString UBlueprintHttpServerLibrary::GetBody(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest)
{
	return HttpRequest.GetBody();
}

bool	UBlueprintHttpServerLibrary::HasUrlParameter(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest, const FString& Name)
{
	return HttpRequest.HasUrlParameter(Name);
}

FString UBlueprintHttpServerLibrary::GetUrlParameter(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest, const FString& Name)
{
	return HttpRequest.GetUrlParameter(Name);
}

FString UBlueprintHttpServerLibrary::GetRemoteAddress(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest)
{
	return HttpRequest.GetRemoteAddress();
}

int32   UBlueprintHttpServerLibrary::GetRemotePort(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest)
{
	return HttpRequest.GetRemotePort();
}

FString UBlueprintHttpServerLibrary::GetVerb(UPARAM(ref) const FBlueprintHttpRequest& HttpRequest)
{
	return HttpRequest.GetVerb();
}

///////////////////////////////////////////////////////
// Async Nodes

UHttpServerListenProxy* UHttpServerListenProxy::Listen(UBlueprintHttpServer* const Server, const FString& Host, const int32 Port)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->Host = Host;
	Proxy->Port = Port;
	Proxy->Server = Server;

	return Proxy;
}

void UHttpServerListenProxy::Activate()
{
	if (!Server)
	{
		FFrame::KismetExecutionMessage(TEXT("HttpServer was nullptr."), ELogVerbosity::Error);
		OnActionOver(false);
	}

	else if (Host.IsEmpty())
	{
		FFrame::KismetExecutionMessage(TEXT("Host can't be empty."), ELogVerbosity::Error);
		OnActionOver(false);
	}

	else if (Port < 0 || Port > 65535)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Port must be in range [0;65535]. Provided %d."), Port), ELogVerbosity::Error);
		OnActionOver(false);
	}
	else
	{
		Server->Listen(MoveTemp(Host), Port, FHttpServerListenCallback::CreateUObject(this, &ThisClass::OnActionOver));
	}
}

void UHttpServerListenProxy::OnActionOver(const bool bSuccess)
{
	(bSuccess ? OnListening : OnFailed).Broadcast();
	SetReadyToDestroy();
}

