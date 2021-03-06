#pragma once
#include "externalLibs.h"
#include "cocoa/file/CPath.h"

typedef unsigned int GLuint;

namespace Cocoa
{
	class COCOA Shader
	{
	public:
		Shader(const CPath& resourceName);

		void Compile(const char* filepath);
		void Bind();
		void Unbind();
		void Delete();

		void UploadVec4(const char* varName, const glm::vec4& vec4);
		void UploadVec3(const char* varName, const glm::vec3& vec3);
		void UploadVec2(const char* varName, const glm::vec2& vec2);
		void UploadFloat(const char* varName, float value);
		void UploadInt(const char* varName, int value);
		void UploadIntArray(const char* varName, int size, int* array);
		void UploadUInt(const char* varName, uint32 value);

		void UploadMat4(const char* varName, const glm::mat4& mat4);
		void UploadMat3(const char* varName, const glm::mat3& mat3);

	private:
		GLuint GetVariableLocation(const char* varName);

	private:
		int m_ShaderProgram;
		bool m_BeingUsed;

		std::unordered_map<const char*, GLuint> m_Variables;
	};
}