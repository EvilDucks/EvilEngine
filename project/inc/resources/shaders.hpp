#pragma once

#include "types.hpp"
#include "scene.hpp"

#include "render/font.hpp"

namespace RESOURCES::SHADERS {

    void Load (
		const u8& directoryLength,
		const char* directory,
		u8*& shadersLoadTable,
		u8*& uniformsTable,
		MATERIAL::Material*& materials
	) {
		PROFILER { ZoneScopedN("GLOBAL: RESOURCES::SHADERS::Load"); }

		char* vertFull = new char[256];
		char* fragFull = new char[256];

		u8 uniformsTableBytesRead = 0;
		u8 stringLength = 0; 
		u64 bytesRead = 0;

		// numof shaders
		const auto& shadersCount = *shadersLoadTable;
		//spdlog::info ("c: {0}", shadersCount);

		for (u8 iShader = 0; iShader < shadersCount; ++iShader) {
			auto& iMaterial = iShader;	// ! If shaders.json then this need to be redone.
			auto& shader = materials[iMaterial].program;

			{ // name
				const auto&& name = (const char*)(shadersLoadTable + 1 + bytesRead);
				for (; name[stringLength] != 0; ++stringLength);
				bytesRead += stringLength + 1;

				//spdlog::info ("{0}, {1}", stringLength, name);
				stringLength = 0;
			}

			// vert
			const auto&& vert = (const char*)(shadersLoadTable + 1 + bytesRead);
			for (; vert[stringLength] != 0; ++stringLength);
			bytesRead += stringLength + 1;

			memcpy (vertFull, directory, directoryLength);					// Get 'Main' directory
			memcpy (vertFull + directoryLength, vert, stringLength);		// Get 'Read' direcotry/filename
			vertFull[directoryLength + stringLength] = 0;					// null-terminate

			//spdlog::info ("{0}, {1}", stringLength, vertFull);
			stringLength = 0;


			// frag
			const auto&& frag = (const char*)(shadersLoadTable + 1 + bytesRead);
			for (; frag[stringLength] != 0; ++stringLength);
			bytesRead += stringLength + 1;
				
			memcpy (fragFull, directory, directoryLength);					// Get 'Main' directory
			memcpy (fragFull + directoryLength, frag, stringLength);		// Get 'Read' direcotry/filename
			fragFull[directoryLength + stringLength] = 0;					// null-terminate

			//spdlog::info ("{0}, {1}", stringLength, fragFull);
			stringLength = 0;

			SHADER::Create (shader, vertFull, fragFull);
			//spdlog::info ("create!");

			// numof uniforms
			const auto& uniformsCount = *(shadersLoadTable + 1 + bytesRead);
			//spdlog::info ("cc: {0}", uniformsCount);
			bytesRead += 1;

			const auto&& uniformsRange = SIZED_BUFFOR::GetCount (uniformsTable, iMaterial, uniformsTableBytesRead);
			auto&& uniforms = (SHADER::UNIFORM::Uniform*)(uniformsRange + 1);
			//const auto& uniformsCount = *(uniformsRange);

			//spdlog::info ("s: {0}", *(uniformsRange));

			for (u8 iUniform = 0; iUniform < uniformsCount; ++iUniform) { // uniform name

				const auto&& name = (const char*)(shadersLoadTable + 1 + bytesRead);
				for (; name[stringLength] != 0; ++stringLength);
				bytesRead += stringLength + 1;

				//spdlog::info ("{0}, {1}", stringLength, name);
				SHADER::UNIFORM::Create (shader, uniforms[iUniform], name);
				stringLength = 0;
			}

			uniformsTableBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;
		}

		delete[] vertFull;
		delete[] fragFull;

		DEBUG_RENDER GL::GetError (1236);
	}


	void LoadSkybox (
		SHADER::Shader& shader
	) {
		SHADER::Create (shader, RESOURCES::MANAGER::SVF_SKYBOX, RESOURCES::MANAGER::SFF_SKYBOX);
	}


	void LogShaders (u8* const& shadersTable) {
		spdlog::info ("SHADERS: {0}", shadersTable[0]);

		u8 uniformsTableBytesRead = 0;
		u8 stringLength = 0; 
		u64 bytesRead = 0;

		for (u8 iShader = 0; iShader < shadersTable[0]; ++iShader) {
			spdlog::info ("s-{0}:", iShader);

			for (u8 i = 0; i < 3; ++i) { // name, vert, frag
				const auto&& name = (const char*)(shadersTable + 1 + bytesRead);
				for (; name[stringLength] != 0; ++stringLength);
				bytesRead += stringLength + 1;

				spdlog::info (" sl: {0}, sn: {1}", stringLength, name);
				stringLength = 0;
			}

			const auto& uniformsCount = *(shadersTable + 1 + bytesRead);
			spdlog::info (" uc: {0}", uniformsCount);
			bytesRead += 1;

			for (u8 iUniform = 0; iUniform < uniformsCount; ++iUniform) { // uniform name

				const auto&& name = (const char*)(shadersTable + 1 + bytesRead);
				for (; name[stringLength] != 0; ++stringLength);
				bytesRead += stringLength + 1;

				spdlog::info (" sl: {0}, sn: {1}", stringLength, name);
				stringLength = 0;
			}

			uniformsTableBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;
		}
	}


	void LogUniforms (u8* const& uniformsTable) {
		auto& shadersCount = uniformsTable[0];
		u8 uniformsTableBytesRead = 0;

		for (u16 iShader = 0; iShader < shadersCount; ++iShader) {
			const auto&& uniformsRange = SIZED_BUFFOR::GetCount (uniformsTable, iShader, uniformsTableBytesRead);
			const auto& uniformsCount = *(uniformsRange);

			auto&& uniforms = (SHADER::UNIFORM::Uniform*)(uniformsRange + 1);
					
			spdlog::info ("UNIFORMS: {0}, s-{1}", uniformsCount, iShader);
			for (u16 iUniform = 0; iUniform < uniformsCount; ++iUniform) {
				auto& uniform = uniforms[iUniform];
				spdlog::info ("u-{0}:", iUniform);
				spdlog::info (" id: {0}", uniform.id);
				spdlog::info (" bi: {0}", uniform.bufforIndex);
				spdlog::info (" si: {0}", uniform.setIndex);
			}

			uniformsTableBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;
		}
	}

}