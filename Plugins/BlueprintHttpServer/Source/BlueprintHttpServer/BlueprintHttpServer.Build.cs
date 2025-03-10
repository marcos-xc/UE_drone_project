// Copyright Pandores Marketplace 2021. All Righst Reserved.

using UnrealBuildTool;
using System.IO;

public class BlueprintHttpServer : ModuleRules
{
	public BlueprintHttpServer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		string HttpLibRoot = Path.Combine(PluginDirectory, "Source/ThirdParty/cpp-httplib/");

		// Engine dependencies
		PublicDependencyModuleNames .AddRange(new string[] { "Core" });		
		PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Engine", "OpenSSL", "zlib" });

		// Module includes
		PublicIncludePaths .Add(Path.Combine(ModuleDirectory, "Public"));
		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));

		// cpp-libhttp
		PrivateIncludePaths.Add(Path.Combine(HttpLibRoot, "include"));

		// cpp-libhttp definitions
		if (PlatformSupportsOpenSSL())
        {
			PublicDefinitions.Add("CPPHTTPLIB_OPENSSL_SUPPORT=1");
		}

		PublicDefinitions.Add("CPPHTTPLIB_ZLIB_SUPPORT=1");

		// HttpServer definitions.
		PublicDefinitions.Add("WITH_BLUEPRINTHTTPSERVER=1");
	}

	private bool PlatformSupportsOpenSSL()
    {
		return
			// iOS and Android doesn't use OpenSSL version 1.1.1+
			Target.Platform != UnrealTargetPlatform.Android &&
			Target.Platform != UnrealTargetPlatform.IOS;
	}
}
