#pragma once

#define Log(format, ...) _Log(__FILE__, __LINE__, format, __VA_ARGS__)
#define Info(format, ...) _Info(__FILE__, __LINE__, format, __VA_ARGS__)
#define Warning(format, ...) _Warning(__FILE__, __LINE__, format, __VA_ARGS__)
#define Error(format, ...) _Error(__FILE__, __LINE__, format, __VA_ARGS__)
#define Assert(condition, format, ...) _Assert(__FILE__, __LINE__, condition, format, __VA_ARGS__)

namespace Cocoa
{
	class COCOA Log
	{
	public:
		static void _Log(const char* filename, int line, const char* format, ...);
		static void _Info(const char* filename, int line, const char* format, ...);
		static void _Warning(const char* filename, int line, const char* format, ...);
		static void _Error(const char* filename, int line, const char* format, ...);
		static void _Assert(const char* filename, int line, int condition, const char* format, ...);
	};
}