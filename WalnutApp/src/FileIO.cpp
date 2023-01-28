#include "FileIO.h"

#include <iostream>
#include <windows.h>
#include <locale>
#include <codecvt>

#include <json/json.h>

namespace  RayMan::FileIO {
	void ConvertRGBAtoRGB(const uint32_t* src, uint8_t* dst, size_t dstBufferSize) {
		const uint32_t* srcPtr = src;
		for (uint8_t* dstPtr; dstPtr < dst + dstBufferSize; dstPtr += 3) {
			uint8_t* croppedSrcPtr = (uint8_t*)srcPtr;
			croppedSrcPtr += 1;
			memcpy(dstPtr, croppedSrcPtr, 3);
		}
	}


	std::wstring OpenFileDialog(const wchar_t* filter) {
		OPENFILENAME ofn = {sizeof(OPENFILENAME)};

		wchar_t szFile[_MAX_PATH] = L"";

		ofn.hwndOwner = GetConsoleWindow();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
		ofn.lpstrFilter = filter;
		ofn.Flags = OFN_FILEMUSTEXIST;

		if (GetOpenFileName(&ofn)) {
			return std::wstring{ofn.lpstrFile};
		}
		return std::wstring{};
	}

	std::wstring SaveFileDialog(const wchar_t* filter, const wchar_t* defaultExt) {
		OPENFILENAME ofn = {sizeof(OPENFILENAME)};

		wchar_t szFile[_MAX_PATH] = L"file";

		ofn.hwndOwner = GetConsoleWindow();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
		ofn.lpstrFilter = filter;
		ofn.lpstrDefExt = defaultExt;
		ofn.Flags = OFN_OVERWRITEPROMPT;

		if (GetSaveFileName(&ofn)) {
			return std::wstring{ofn.lpstrFile};
		}
		return std::wstring{};
	}

	void WriteImage(const std::string& filename, int width, int height, const uint32_t* buffer) {
		std::ofstream file{filename, std::ios_base::out | std::ios_base::binary};
		file << "P6\n"
			<< width << " " << height << '\n'
			<< 255 << "\n";

		for (int y{height - 1}; y >= 0; y--) {
			for (int x{0}; x < width; x++) {
				uint8_t* pixelComponents{(uint8_t*)&buffer[x + y * width]};
				file << (char)(pixelComponents[0]) << (char)(pixelComponents[1]) << (char)(pixelComponents[2]);
			}
		}

		file.close();
	}

	Json::Value MakeSphereJson(const ::RayMan::Sphere& sphere) {
		Json::Value result;
		result["position"]["x"] = sphere.Position.x;
		result["position"]["y"] = sphere.Position.y;
		result["position"]["z"] = sphere.Position.z;

		result["materialIndex"] = sphere.MaterialIndex;
		result["radius"] = sphere.Radius;

		return result;
	}

	Json::Value MakeDirectionalLightJson(const::RayMan::DirectionalLight& light) {
		Json::Value result;
		glm::vec3 color = light.Color();
		result["color"]["r"] = color.r;
		result["color"]["g"] = color.g;
		result["color"]["b"] = color.b;

		glm::vec2 rotation = light.Rotation();
		result["rotation"]["yaw"] = rotation.x;
		result["rotation"]["pitch"] = rotation.y;

		return result;
	}

	Json::Value MakeRenderPipelineJson(const::RayMan::Renderer::RenderPipeline& pipeline) {
		Json::Value result;

		result["frameLimit"] = pipeline.FrameLimit;
		Renderer::Settings settings = pipeline.Settings;
		result["settings"]["accumulate"] = settings.Accumulate;
		result["settings"]["antialising"] = settings.Antialising;
		result["settings"]["bounce"] = settings.Bounce;
		result["settings"]["shadows"] = settings.CastShadows;

		return result;
	}

	void SaveScene(const std::string& filename, const ::RayMan::Scene& scene) {
		std::ofstream file{filename};

		if (!file.is_open()) {
			throw std::runtime_error("Could not open file " + filename);
		}

		file << MakeSceneJson(scene);
		file.close();
	}

	void LoadScene(const std::string& filename, ::RayMan::Scene& scene) {
		std::ifstream file{filename};

		if (!file.is_open()) {
			throw std::runtime_error("Could not open file " + filename);
		}

		Json::Value json;
		file >> json;

		ParseSceneJson(scene, json);
	}

	void SaveRenderPipelines(const std::string& filename, const std::vector<::RayMan::Renderer::RenderPipeline>& pipelines) {
		std::ofstream file{filename};
		if (!file.is_open()) {
			throw std::runtime_error("Could not open file" + filename);
		}

		Json::Value json;
		for (int i{0}; i < pipelines.size(); i++) {
			json[i] = MakeRenderPipelineJson(pipelines[i]);
		}

		file << json;
	}

	void LoadRenderPipelines(const std::string& filename, std::vector<::RayMan::Renderer::RenderPipeline>& pipelines) {
		std::ifstream file{filename};

		if (!file.is_open()) {
			throw std::runtime_error("Could not open file" + filename);
		}

		Json::Value json;
		file >> json;

		for (int i{0}; i < json.size(); i++) {
			ParseRenderPiplineJson(pipelines[i], json[i]);
		}

		file.close();
	}

	std::string ConvertToStr(const std::wstring& wstring) {
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.to_bytes(wstring);
	}

	std::wstring ConvertToWstr(const std::string& string) {
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.from_bytes(string);
	}

	Json::Value MakeSceneJson(const::RayMan::Scene& scene) {
		Json::Value root;
		for (int i{0}; i < scene.Spheres.size(); i++) {
			root["spheres"][i] = MakeSphereJson(scene.Spheres[i]);
		}
		for (int i{0}; i < scene.DirectionalLights.size(); i++) {
			root["directionalLights"][i] = MakeDirectionalLightJson(scene.DirectionalLights[i]);
		}
		for (int i{0}; i < scene.Materials.size(); i++) {
			root["materials"][i] = MakeMaterialJson(scene.Materials[i]);
		}

		glm::vec3 skyBaseColor = scene.SkyLightBaseColor;

		root["skyLightBaseColor"]["r"] = skyBaseColor.r;
		root["skyLightBaseColor"]["g"] = skyBaseColor.g;
		root["skyLightBaseColor"]["b"] = skyBaseColor.b;

		glm::vec3 skySecondaryColor = scene.SkyLightSecondaryColor;

		root["skyLightSecondaryColor"]["r"] = skySecondaryColor.r;
		root["skyLightSecondaryColor"]["g"] = skySecondaryColor.g;
		root["skyLightSecondaryColor"]["b"] = skySecondaryColor.b;

		return root;
	}

	Json::Value MakeMaterialJson(const ::RayMan::Material& material) {
		Json::Value result;

		glm::vec3 albedo = material.Albedo;
		result["albedo"]["r"] = albedo.r;
		result["albedo"]["g"] = albedo.g;
		result["albedo"]["b"] = albedo.b;

		result["roughness"] = material.Roughness;

		return result;
	}

	void ParseSceneJson(::RayMan::Scene& scene, const Json::Value& sceneJson) {
		scene.Spheres.resize(sceneJson["spheres"].size());
		for (int i{0}; i < sceneJson["spheres"].size(); i++) {
			ParseSphereJson(scene.Spheres[i], sceneJson["spheres"][i]);
		}

		scene.DirectionalLights.resize(sceneJson["directionalLights"].size());
		for (int i{0}; i < sceneJson["directionalLights"].size(); i++) {
			ParseDirectionalLightJson(scene.DirectionalLights[i], sceneJson["directionalLights"][i]);
		}

		scene.Materials.resize(sceneJson["materials"].size());
		for (int i{0}; i < sceneJson["spheres"].size(); i++) {
			ParseMaterialJson(scene.Materials[i], sceneJson["materials"][i]);
		}
	}

	void ParseSphereJson(::RayMan::Sphere& sphere, const Json::Value& sphereJson) {
		sphere.Position.x = sphereJson["position"]["x"].asFloat();
		sphere.Position.y = sphereJson["position"]["y"].asFloat();
		sphere.Position.z = sphereJson["position"]["z"].asFloat();

		sphere.MaterialIndex = sphereJson["materialIndex"].asInt();
		sphere.Radius = sphereJson["radius"].asFloat();
	}

	void ParseDirectionalLightJson(::RayMan::DirectionalLight& light, const Json::Value& lightJson) {
		glm::vec3& color{light.Color()};
		color.r = lightJson["color"]["r"].asFloat();
		color.g = lightJson["color"]["g"].asFloat();
		color.b = lightJson["color"]["b"].asFloat();

		glm::vec2 rotation;
		rotation.x = lightJson["rotation"]["yaw"].asFloat();
		rotation.y = lightJson["rotation"]["pitch"].asFloat();
		light.SetRotation(rotation);
	}

	void ParseRenderPiplineJson(::RayMan::Renderer::RenderPipeline& pipeline, const Json::Value& pipelineJson) {
		pipeline.FrameLimit = pipelineJson["frameLimit"].asInt();

		Renderer::Settings& settings{pipeline.Settings};

		settings.Accumulate = pipelineJson["settings"]["accumulate"].asBool();
		settings.Antialising = pipelineJson["settings"]["antialising"].asBool();
		settings.Bounce = pipelineJson["settings"]["bounce"].asInt();
		settings.CastShadows = pipelineJson["settings"]["shadows"].asBool();
	}

	void ParseMaterialJson(::RayMan::Material& material, const Json::Value& materialJson) {

		glm::vec3& albedo{material.Albedo};
		albedo.r = materialJson["albedo"]["r"].asFloat();
		albedo.g = materialJson["albedo"]["g"].asFloat();
		albedo.b = materialJson["albedo"]["b"].asFloat();

		material.Roughness = materialJson["roughness"].asFloat();
	}
}
