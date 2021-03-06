#pragma once

#include "cocoa/physics2d/Physics2D.h"
#include "cocoa/core/Core.h"
#include "cocoa/core/AssetManager.h"

namespace Cocoa
{
	class COCOA Texture : public Asset
	{
	public:
		Texture(const CPath& resourceName, bool isDefault=false);
		Texture(int width, int height, bool isDefault=false);

		virtual void Load() override;
		virtual void Unload() override;

		void Bind();
		void Unbind();

		void FreePixels();
		const uint8* GetPixelBuffer();

		inline int GetId() const { return m_ID; }
		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
		inline const CPath& GetFilepath() const { return m_Path; }
		inline int BytesPerPixel() const { return m_BytesPerPixel; }
		inline bool IsDefault() { return m_IsDefault; }

	private:
		unsigned int m_ID;
		int m_Width, m_Height;
		int m_BytesPerPixel = 3;

		uint8* m_PixelBuffer = nullptr;
		bool m_PixelsFreed = false;

		AABB m_BoundingBox;
	};
}