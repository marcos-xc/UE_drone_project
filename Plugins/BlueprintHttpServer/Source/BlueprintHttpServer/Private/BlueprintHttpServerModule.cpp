// Copyright Pandores Marketplace 2021. All Righst Reserved.

#include "BlueprintHttpServerModule.h"

DEFINE_LOG_CATEGORY(LogHttpServer);

#define LOCTEXT_NAMESPACE "FBlueprintHttpServerModule"

void FBlueprintHttpServerModule::StartupModule()
{
	
}

void FBlueprintHttpServerModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBlueprintHttpServerModule, BlueprintHttpServer)