----------------------------------------------------------------------
-- Premake4 configuration script for OpenDE
-- Contributed by Jason Perkins (starkos@industriousone.com)
-- For more information on Premake: http://industriousone.com/premake
----------------------------------------------------------------------

----------------------------------------------------------------------
-- Demo list: add/remove demos from here and the rest of the build
-- should just work.
----------------------------------------------------------------------

  local demos = {
    "boxstack",
    "buggy",
    "cards",
    "chain1",
    "chain2",
    "collision",
    "crash",
    "cylvssphere",
    "feedback",
    "friction",
    "gyroscopic",
    "heightfield",
    "hinge",
    "I",
    "jointPR",
    "jointPU",
    "joints",
    "kinematic",
    "motion",
    "motor",
    "ode",
    "piston",
    "plane2d",
    "slider",
    "space",
    "space_stress",
    "step",
  }

  local trimesh_demos = {
    "basket",
    "cyl",
    "moving_trimesh",
    "trimesh",
  }
  
  if not _OPTIONS["no-trimesh"] then
    demos = table.join(demos, trimesh_demos)
  end



----------------------------------------------------------------------
-- Configuration options
----------------------------------------------------------------------

  newoption {
    trigger     = "with-demos",
    description = "Builds the demo applications and DrawStuff library"
  }
  
  newoption {
    trigger     = "with-tests",
    description = "Builds the unit test application"
  }
  
  newoption {
    trigger     = "with-gimpact",
    description = "Use GIMPACT for trimesh collisions (experimental)"
  }
  
  newoption {
    trigger     = "all-collis-libs",
    description = "Include sources of all collision libraries into the project"
  }
  
  newoption {
    trigger     = "no-dif",
    description = "Exclude DIF (Dynamics Interchange Format) exports"
  }
  
  newoption {
    trigger     = "no-trimesh",
    description = "Exclude trimesh collision geometry"
  }
  
  newoption {
    trigger     = "no-alloca",
    description = "Use heap memory instead of the stack (experimental)"
  }
  
  newoption {
    trigger     = "enable-ou",
    description = "Use TLS for global variables (experimental)"
  }

  newoption {
    trigger     = "16bit-indices",
    description = "Use 16-bit indices for trimeshes (default is 32-bit)"
  }

  newoption {
    trigger     = "old-trimesh",
    description = "Use old OPCODE trimesh-trimesh collider"
  }
  
  newoption {
    trigger = "to",
    value   = "path",
    description = "Set the output location for the generated project files"
  }

  -- always clean all of the optional components and toolsets
  if _ACTION == "clean" then
    _OPTIONS["with-demos"] = ""
    _OPTIONS["with-tests"] = ""
    for action in pairs(premake.actions) do
      os.rmdir(action)
    end
  end
  
  

----------------------------------------------------------------------
-- The solution, and solution-wide settings
----------------------------------------------------------------------

  solution "ode"

    language "C++"
    uuid     "4DA77C12-15E5-497B-B1BB-5100D5161E15"
    location ( _OPTIONS["to"] or _ACTION )

    includedirs {
      "../include",
      "../ode/src"
    }
      
    -- define all the possible build configurations
    configurations {
      "DebugSingleDLL", "ReleaseSingleDLL", 
      "DebugSingleLib", "ReleaseSingleLib", 
      "DebugDoubleDLL", "ReleaseDoubleDLL", 
      "DebugDoubleLib", "ReleaseDoubleLib"    
    }
    
    configuration { "Debug*" }
      defines { "_DEBUG" }
      flags   { "Symbols" }
      
    configuration { "Release*" }
      flags   { "OptimizeSpeed", "NoFramePointer" }

    configuration { "*Single*" }
      defines { "dSINGLE" }
      
    configuration { "*Double*" }
      defines { "dDOUBLE" }
    
    configuration { "Windows" }
      defines { "WIN32" }

    configuration { "MacOSX" }
      linkoptions { "-framework Carbon" }
      
    -- give each configuration a unique output directory
    for _, name in ipairs(configurations()) do
      configuration { name }
        targetdir ( "../lib/" .. name )
    end
      
    -- disable Visual Studio security warnings
    configuration { "vs*" }
      defines { "_CRT_SECURE_NO_DEPRECATE" }

    -- don't remember why we had to do this	
    configuration { "vs2002 or vs2003", "*Lib" }
      flags  { "StaticRuntime" }



----------------------------------------------------------------------
-- The demo projects, automated from list above. These go first so
-- they will be selected as the active project automatically in IDEs
----------------------------------------------------------------------

  if _OPTIONS["with-demos"] then
    for _, name in ipairs(demos) do
    
      project ( "demo_" .. name )
      
        kind      "ConsoleApp"
        location  ( _OPTIONS["to"] or _ACTION )
        files     { "../ode/demo/demo_" .. name .. ".*" }
		links     { "ode", "drawstuff" }        
        
        configuration { "Windows" }
          files   { "../drawstuff/src/resources.rc" }
          links   { "user32", "winmm", "gdi32", "opengl32", "glu32" }

        configuration { "MacOSX" }
          linkoptions { "-framework Carbon -framework OpenGL -framework AGL" }

        configuration { "not Windows", "not MacOSX" }
          links   { "GL", "GLU" }
        
    end
  end
  


----------------------------------------------------------------------
-- The ODE library project
----------------------------------------------------------------------

  project "ode"

    kind     "StaticLib"
    location ( _OPTIONS["to"] or _ACTION )

    includedirs {
      "../ode/src/joints",
      "../OPCODE",
      "../GIMPACT/include",
      "../ou/include"
    }

    files {
      "../include/ode/*.h",
      "../ode/src/joints/*.h", 
      "../ode/src/joints/*.cpp",
      "../ode/src/*.h", 
      "../ode/src/*.c", 
      "../ode/src/*.cpp",
    }

    excludes {
      "../ode/src/collision_std.cpp",
    }

    configuration { "no-dif" }
      excludes { "../ode/src/export-dif.cpp" }

    configuration { "no-trimesh" }
      excludes {
        "../ode/src/collision_trimesh_colliders.h",
        "../ode/src/collision_trimesh_internal.h",
        "../ode/src/collision_trimesh_opcode.cpp",
        "../ode/src/collision_trimesh_gimpact.cpp",
        "../ode/src/collision_trimesh_box.cpp",
        "../ode/src/collision_trimesh_ccylinder.cpp",
        "../ode/src/collision_cylinder_trimesh.cpp",
        "../ode/src/collision_trimesh_distance.cpp",
        "../ode/src/collision_trimesh_ray.cpp",
        "../ode/src/collision_trimesh_sphere.cpp",
        "../ode/src/collision_trimesh_trimesh.cpp",
        "../ode/src/collision_trimesh_plane.cpp"
      }

    configuration { "not no-trimesh", "with-gimpact or all-collis-libs" }
      files   { "../GIMPACT/**.h", "../GIMPACT/**.cpp" }
    
    configuration { "not no-trimesh", "not with-gimpact" }
      files   { "../OPCODE/**.h", "../OPCODE/**.cpp" }
 
    configuration { "enable-ou" }
      files   { "../ou/**.h", "../ou/**.cpp" }
      defines { "_OU_NAMESPACE=odeou" }
      
    configuration { "windows" }
      links   { "user32" }
            
    configuration { "*Lib" }
      kind    "StaticLib"
      defines "ODE_LIB"
      
    configuration { "*DLL" }
      kind    "SharedLib"
      defines "ODE_DLL"

    configuration { "DebugSingle*" }
      targetname "ode_singled"
      defines    "dSINGLE"
      
    configuration { "ReleaseSingle*" }
      targetname "ode_single"
      defines    "dSINGLE"
      
    configuration { "DebugDouble*" }
      targetname "ode_doubled"
      defines    "dDOUBLE"
      
    configuration { "ReleaseDouble*" }
      targetname "ode_double"
      defines    "dDOUBLE"


----------------------------------------------------------------------
-- Write a custom <config.h> to src/ode, based on the supplied flags
----------------------------------------------------------------------

  if _ACTION then
    io.input("config-default.h")
    local text = io.read("*a")

    if _OPTIONS["no-trimesh"] then
      text = string.gsub(text, "#define dTRIMESH_ENABLED 1", "/* #define dTRIMESH_ENABLED 1 */")
      text = string.gsub(text, "#define dTRIMESH_OPCODE 1", "/* #define dTRIMESH_OPCODE 1 */")
    elseif (_OPTIONS["with-gimpact"]) then
      text = string.gsub(text, "#define dTRIMESH_OPCODE 1", "#define dTRIMESH_GIMPACT 1")
    end

    if _OPTIONS["no-alloca"] then
      text = string.gsub(text, "/%* #define dUSE_MALLOC_FOR_ALLOCA %*/", "#define dUSE_MALLOC_FOR_ALLOCA")
    end

    if _OPTIONS["enable-ou"] then
      text = string.gsub(text, "/%* #define dOU_ENABLED 1 %*/", "#define dOU_ENABLED 1")
      text = string.gsub(text, "/%* #define dATOMICS_ENABLED 1 %*/", "#define dATOMICS_ENABLED 1")
      text = string.gsub(text, "/%* #define dTLS_ENABLED 1 %*/", "#define dTLS_ENABLED 1")
    end

    if _OPTIONS["16bit-indices"] then
      text = string.gsub(text, "#define dTRIMESH_16BIT_INDICES 0", "#define dTRIMESH_16BIT_INDICES 1")
    end
  
    if _OPTIONS["old-trimesh"] then
      text = string.gsub(text, "#define dTRIMESH_OPCODE_USE_OLD_TRIMESH_TRIMESH_COLLIDER 0", "#define dTRIMESH_OPCODE_USE_OLD_TRIMESH_TRIMESH_COLLIDER 1")
    end
    
    io.output("../ode/src/config.h")
    io.write(text)
    io.close()
  end


----------------------------------------------------------------------
-- The DrawStuff library project
----------------------------------------------------------------------

  if _OPTIONS["with-demos"] then

    project "drawstuff"

      location ( _OPTIONS["to"] or _ACTION )

      files {
        "../include/drawstuff/*.h",
        "../drawstuff/src/internal.h",
        "../drawstuff/src/drawstuff.cpp"
      }
    
      configuration { "Debug*" }
        targetname "drawstuffd"
            
      configuration { "*Lib" }
        kind    "StaticLib"
        defines { "DS_LIB" }
      
      configuration { "*DLL" }
        kind    "SharedLib"
        defines { "DS_DLL", "USRDLL" }
      
      configuration { "Windows" }
        files   { "../drawstuff/src/resource.h", "../drawstuff/src/resources.rc", "../drawstuff/src/windows.cpp" }
        links   { "user32", "opengl32", "glu32", "winmm", "gdi32" }

      configuration { "MacOSX" }
	    defines     { "HAVE_APPLE_OPENGL_FRAMEWORK" }
        files       { "../drawstuff/src/osx.cpp" }
        linkoptions { "-framework Carbon -framework OpenGL -framework AGL" }

      configuration { "not Windows", "not MacOSX" }
        files   { "../drawstuff/src/x11.cpp" }
        links   { "X11", "GL", "GLU" }

  end
  


----------------------------------------------------------------------
-- The automated test application
----------------------------------------------------------------------


  if _OPTIONS["with-tests"] then
  
    project "tests"
  
      kind     "ConsoleApp"
      location ( _OPTIONS["to"] or _ACTION )

      includedirs { 
        "../tests/UnitTest++/src" 
      }
    
      files { 
        "../tests/*.cpp", 
        "../tests/joints/*.cpp", 
        "../tests/UnitTest++/src/*" 
      }

      links { "ode" }
    
      configuration { "Windows" }
        files { "../tests/UnitTest++/src/Win32/*" }
      
      configuration { "not Windows" }
        files { "../tests/UnitTest++/src/Posix/*" }

      -- add post-build step to automatically run test executable
      local path_to_lib = path.getrelative(location(), "../lib")
      local command = path.translate(path.join(path_to_lib, "%s/tests"))
      
      for _, name in ipairs(configurations()) do
        configuration { name }
          postbuildcommands { command:format(name) }
      end

  end

