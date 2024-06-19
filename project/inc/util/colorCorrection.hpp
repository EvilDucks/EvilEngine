#pragma once
#include "tool/debug.hpp"
#include "render/shader.hpp"

namespace COLORCORRECTION {

	r32 gBrightness = 0.05f;	// -1.0 - 1.0
	r32 gContrast 	= 1.2f; 	// 0.0 - 2.0
	r32 gExposure 	= 0.9f;		// 0.0 - 2.0
	r32 gSaturation = 0.5f;		// 0.0 - 2.0

	void ApplyBrightness (
		/* OUT */ glm::mat4& mat, 
		/* OUT */ SHADER::UNIFORM::F4& off, 
		/* IN  */ const float& brightness
	) {
		off.v1 = brightness;
		off.v2 = brightness;
		off.v3 = brightness;
	}

	void ApplyContrast (
		/* OUT */ glm::mat4& mat, 
		/* OUT */ SHADER::UNIFORM::F4& off, 
		/* IN  */ const float& contrast
	) {
		const float dContrast = (1.0f - contrast) / 2.0f;
		
		mat[0][0] *= contrast;
		mat[1][1] *= contrast;
		mat[2][2] *= contrast;
		off.v1 *= dContrast;
		off.v2 *= dContrast;
		off.v3 *= dContrast;
	}
		
	void ApplyExposure (
		/* OUT */ glm::mat4& mat, 
		/* OUT */ SHADER::UNIFORM::F4& off, 
		/* IN  */ const float& exposure
	) {
		mat[0][0] *= exposure;
		mat[1][1] *= exposure;
		mat[2][2] *= exposure;
	}

	// If zero return '1' else return 'in'
	constexpr r32 ZeroCheck (const r32& in) {
		return ((in != 0.0f) * in) + (in == 0.0f);
	}

	void ApplySaturation (
		/* OUT */ glm::mat4& mat, 
		/* OUT */ SHADER::UNIFORM::F4& off, 
		/* IN  */ const float& saturation
	) {
		// https://www.w3.org/TR/WCAG20/#relativeluminancedef
		const float lr = 0.2126f;
		const float lg = 0.7152f;
		const float lb = 0.0722f;

		const float sr = (1 - saturation) * lr;
		const float sg = (1 - saturation) * lg;
		const float sb = (1 - saturation) * lb;

		mat[0][0] *= sr + saturation;
		mat[0][1] = ZeroCheck(mat[0][1]) * sr;
		mat[0][2] = ZeroCheck(mat[0][2]) * sr;
		mat[1][0] = ZeroCheck(mat[1][0]) * sg;
		mat[1][1] *= sg + saturation;
		mat[1][2] = ZeroCheck(mat[1][2]) * sg;
		mat[2][0] = ZeroCheck(mat[2][0]) * sb;
		mat[2][1] = ZeroCheck(mat[2][1]) * sb;
		mat[2][2] *= sb + saturation;
	}

	void ApplyCustomFilter (
		/* OUT */ glm::mat4& mat, 
		/* OUT */ SHADER::UNIFORM::F4& off
	) {
    	mat[0][0] = 1.000; mat[1][0] = 0.500; mat[2][0] = 0.000; mat[3][0] = 0.000; off.v1 = 0.000;
    	mat[0][1] = 0.000; mat[1][1] = 1.000; mat[2][1] = 0.500; mat[3][1] = 0.000; off.v2 = 0.000;
    	mat[0][2] = 0.000; mat[1][2] = 0.000; mat[2][2] = 1.000; mat[3][2] = 0.000; off.v3 = 0.000;
    	mat[0][3] = 0.000; mat[1][3] = 0.500; mat[2][3] = 0.500; mat[3][3] = 1.000; off.v4 = 0.000;
	}
	

}
