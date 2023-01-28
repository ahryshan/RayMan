#pragma once

#include <fstream>
#include <string>
#include <json/json.h>

#include "Scene.h"
#include "Renderer.h"

namespace RayMan::FileIO {
	std::wstring OpenFileDialog(const wchar_t* filter);
	std::wstring SaveFileDialog(const wchar_t* filter, const wchar_t* defaultExt);

	void WriteImage(const std::string& filename, int width, int height, const uint32_t* buffer);

	void SaveScene(const std::string& filename, const ::RayMan::Scene& scene);
	void LoadScene(const std::string& filename, ::RayMan::Scene& scene);

	void SaveRenderPipelines(const std::string& filename, const std::vector<::RayMan::Renderer::RenderPipeline>& pipelines);
	void LoadRenderPipelines(const std::string& filename, std::vector<::RayMan::Renderer::RenderPipeline>& pipelines);

	std::string ConvertToStr(const std::wstring& wstring);
	std::wstring ConvertToWstr(const std::string& string);

	Json::Value MakeSceneJson(const ::RayMan::Scene& scene);
	Json::Value MakeSphereJson(const ::RayMan::Sphere& sphere);
	Json::Value MakeDirectionalLightJson(const ::RayMan::DirectionalLight& light);
	Json::Value MakeRenderPipelineJson(const ::RayMan::Renderer::RenderPipeline& pipeline);
	Json::Value MakeMaterialJson(const ::RayMan::Material& material);

	void ParseSceneJson(::RayMan::Scene& scene, const Json::Value& sceneJson);
	void ParseSphereJson(::RayMan::Sphere& sphere, const Json::Value& sphereJson);
	void ParseDirectionalLightJson(::RayMan::DirectionalLight& light, const Json::Value& lightJson);
	void ParseRenderPiplineJson(::RayMan::Renderer::RenderPipeline& pipeline, const Json::Value& pipelineJson);
	void ParseMaterialJson(::RayMan::Material& material, const Json::Value& materialJson);
}