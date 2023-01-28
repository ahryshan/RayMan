-- premake5.lua
workspace "RayMan"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "RayMan"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "WalnutExternal.lua"
include "RayMan"