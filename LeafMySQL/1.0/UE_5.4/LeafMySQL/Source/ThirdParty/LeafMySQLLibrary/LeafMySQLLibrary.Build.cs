// Copyright Ye RongZhen(MapleLeaf_Ye) 2024

using System.IO;
using UnrealBuildTool;

public class LeafMySQLLibrary : ModuleRules
{
	public LeafMySQLLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		//PublicSystemIncludePaths.Add("$(ModuleDir)/Public");

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
            PublicSystemIncludePaths.Add(Path.Combine(ModuleDirectory, "x64", "Release", "lib"));
            PublicSystemIncludePaths.Add(Path.Combine(ModuleDirectory, "x64", "Release", "include"));
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "x64", "Release", "lib", "mysqlcppconn-static.lib"));
            //必须按照这个顺序依赖否则导致无法挂载先libcrypto再libssl
            PublicDelayLoadDLLs.Add("libcrypto-1_1-x64.dll");
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/LeafMySql/Win64/libcrypto-1_1-x64.dll");

            PublicDelayLoadDLLs.Add("libssl-1_1-x64.dll");
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/LeafMySql/Win64/libssl-1_1-x64.dll");
            

        }
		//else if (Target.Platform == UnrealTargetPlatform.Mac)
		//{
		//	//PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libExampleLibrary.dylib"));
		//	//RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/LeafMySQLLibrary/Mac/Release/libExampleLibrary.dylib");
		//}
		//else if (Target.Platform == UnrealTargetPlatform.Linux)
		//{
		//	//string ExampleSoPath = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "LeafMySQLLibrary", "Linux", "x86_64-unknown-linux-gnu", "libExampleLibrary.so");
		//	//PublicAdditionalLibraries.Add(ExampleSoPath);
		//	//PublicDelayLoadDLLs.Add(ExampleSoPath);
		//	//RuntimeDependencies.Add(ExampleSoPath);
		//}
	}
}
