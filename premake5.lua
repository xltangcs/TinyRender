workspace "TinyRenderer"		-- sln文件名
	architecture "x64"	
	configurations{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}
-- outputdir = Debug-windows-x86_64
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


project "TinyRenderer"		
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}") 
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files{
		"src/**.h",
		"src/**.cpp"
	}
	

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "CANDY_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "CANDY_RELEASE"
		runtime "Release"
		optimize "on"
