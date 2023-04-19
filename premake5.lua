workspace "Rapier"
	architecture "x64"

	configurations{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Rapier/vendor/GLFW/include"
IncludeDir["Glad"] = "Rapier/vendor/Glad/include"
IncludeDir["ImGui"] = "Rapier/vendor/ImGui"
IncludeDir["glm"] = "Rapier/vendor/glm"
IncludeDir["stb"] = "Rapier/vendor/stb"

include "Rapier/vendor/GLFW"
include "Rapier/vendor/Glad"
include "Rapier/vendor/ImGui"

project "Rapier"
	location "Rapier"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-itm/" .. outputdir .. "/%{prj.name}")

	pchheader "ipch.h"
	pchsource "Rapier/src/ipch.cpp"

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/RapierEntryPoint.h",
		"%{prj.name}/Rapier.h",
		"%{prj.name}/res/**",
		"%{prj.name}/vendor/stb/**.h",
		"%{prj.name}/vendor/stb/**.cpp",
	}

	includedirs{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb}"
	}

	links{
		"Glad",
		"GLFW",
		"ImGui",
		"opengl32.lib"
	}


	systemversion "latest"
	defines{
		"RAPIER_BUILD_DLL",
		"GLFW_INCLUDE_NONE",
		"_CRT_SECURE_NO_WARNINGS"
	}

	filter "configurations:Debug"
		defines "RAPIER_DEBUG"
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Release"
		defines "RAPIER_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "RAPIER_DIST"
		runtime "Release"
		optimize "on"

	

project "Forehead"
	location "Forehead"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "On"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-itm/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/res/**",
	}

	includedirs{
		"%{prj.name}/src",
		"Rapier/vendor/spdlog/include",
		"Rapier",
		"Rapier/src",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}"
	}
	 
	links{
		"Rapier"
	}


	systemversion "latest"

	filter "configurations:Debug"
		defines "RAPIER_DEBUG"
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Release"
		defines "RAPIER_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "RAPIER_DIST"
		runtime "Release"
		optimize "on"