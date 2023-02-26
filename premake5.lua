workspace "GraphicsAssignment"
    architecture "x64"
    configurations { "Debug", "Release"}

outputdir = "%{cfg.buildcfg}-%{cfg.system}"



Libs = {}
Libs["glew32"] = "3rdparty/lib/glew32.lib"
Libs["SDL2"] = "3rdparty/lib/SDL2.lib"
Libs["SDL_image"] = "3rdparty/lib/SDL2_image.lib"
Libs["SDLmain"] = "3rdparty/lib/SDL2main.lib"

project "GraphicsAssignment"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "Source/**.h",
        "Source/**.hpp",
        "Source/**.cpp"
    }

    includedirs {
        "3rdparty/includes"
    }

    links {
        "%{Libs.glew32}",
        "%{Libs.SDL2}",
        "%{Libs.SDL_image}",
        "%{Libs.SDLmain}",
    }

    debugdir "%{wks.location}"

    postbuildcommands
	{
		("{COPY} %{wks.location}3rdparty\\bin\\**.dll %{wks.location}bin\\" .. outputdir .. "\\GraphicsAssignment"),
		--("{COPY} %{wks.location}bin\\" .. outputdir .. "\\%{prj.name}\\**.lib %{wks.location}bin\\" .. outputdir .. "\\CupOfTea"),
	}

    filter "system:windows"
		systemversion "latest"
        links
		{
			"opengl32",
		}

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
		runtime "Release"
		optimize "on"
        symbols "off"