#include "externalLibs.h"

#include "cocoa/systems/RenderSystem.h"
#include "cocoa/components/components.h"
#include "cocoa/renderer/Shader.h"
#include "cocoa/core/Application.h"

namespace Cocoa
{
	bool RenderBatch::Compare(const std::shared_ptr<RenderBatch>& b1, const std::shared_ptr<RenderBatch>& b2)
	{
		return b1->ZIndex() < b2->ZIndex();
	}

	RenderBatch::RenderBatch(int maxBatchSize, int zIndex, bool batchOnTop)
	{
		m_ZIndex = zIndex;
		m_MaxBatchSize = maxBatchSize;
		m_VertexBufferBase = new Vertex[m_MaxBatchSize * 4];
		m_VertexStackPointer = &m_VertexBufferBase[0];
		m_Indices = new uint32[m_MaxBatchSize * 6];

		for (int i = 0; i < m_Textures.size(); i++)
		{
			m_Textures[i] = TextureHandle::null;
		}

		m_VAO = -1;
		m_VBO = -1;
		m_EBO = -1;

		m_BatchOnTop = batchOnTop;
	}

	void RenderBatch::Start()
	{
		this->GenerateIndices();

		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);

		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4 * m_MaxBatchSize, nullptr, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 6 * m_MaxBatchSize, this->m_Indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, sizeof(Vertex().position) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, sizeof(Vertex().color) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, color));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, sizeof(Vertex().texCoords) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, sizeof(Vertex().texId) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, texId));
		glEnableVertexAttribArray(3);

		// As long as our struct contains 32-bit members only, the second parameter should be correct
		glVertexAttribIPointer(4, sizeof(Vertex().texId) / sizeof(float), GL_UNSIGNED_INT, sizeof(Vertex), (void*)offsetof(Vertex, entityId));
		glEnableVertexAttribArray(4);
	}

	void RenderBatch::Add(const Transform& transform, const SpriteRenderer& spr)
	{
		m_NumSprites++;

		const Sprite& sprite = spr.m_Sprite;
		TextureHandle tex = sprite.m_Texture;
		if (tex != TextureHandle::null)
		{
			if (!HasTexture(tex))
			{
				m_Textures[m_NumTextures] = tex;
				m_NumTextures++;
			}
		}

		LoadVertexProperties(transform, spr);
	}

	void RenderBatch::Add(const glm::vec2& min, const glm::vec2& max, const glm::vec3& color)
	{
		m_NumSprites++;
		std::array<glm::vec2, 4> texCoords{
			glm::vec2 {1, 1},
			glm::vec2 {1, 0},
			glm::vec2 {0, 0},
			glm::vec2 {0, 1}
		};
		glm::vec4 vec4Color{ color.x, color.y, color.z, 1.0f };
		glm::vec2 quadSize{ max.x - min.x, max.y - min.y };
		glm::vec3 position{ min.x + ((max.x - min.x) / 2.0f), min.y + ((max.y - min.y) / 2.0f), 0.0f };
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		int texId = 0;
		float rotation = 0.0f;

		LoadVertexProperties(position, scale, quadSize, &texCoords[0], rotation, vec4Color, texId);
	}

	void RenderBatch::Add(const glm::vec2* vertices, const glm::vec3& color)
	{
		m_NumSprites++;
		std::array<glm::vec2, 4> texCoords{
			glm::vec2 {1, 1},
			glm::vec2 {1, 0},
			glm::vec2 {0, 0},
			glm::vec2 {0, 1}
		};
		glm::vec4 vec4Color{ color.x, color.y, color.z, 1.0f };
		int texId = 0;

		LoadVertexProperties(vertices, &texCoords[0], vec4Color, texId);
	}

	void RenderBatch::Add(TextureHandle textureHandle, const glm::vec2& size, const glm::vec2& position,
		const glm::vec3& color, const glm::vec2& texCoordMin, const glm::vec2& texCoordMax, float rotation)
	{
		m_NumSprites++;
		std::array<glm::vec2, 4> texCoords{
			glm::vec2 {texCoordMax.x, texCoordMax.y},
			glm::vec2 {texCoordMax.x, texCoordMin.y},
			glm::vec2 {texCoordMin.x, texCoordMin.y},
			glm::vec2 {texCoordMin.x, texCoordMax.y}
		};
		glm::vec4 vec4Color{ color.x, color.y, color.z, 1.0f };
		glm::vec3 vec3Pos{ position.x, position.y, 0.0f };
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };


		if (!HasTexture(textureHandle))
		{
			m_Textures[m_NumTextures] = textureHandle;
			m_NumTextures++;
		}

		int texId = 0;
		for (int i = 0; i < m_NumTextures; i++)
		{
			if (m_Textures[i] == textureHandle)
			{
				texId = i + 1;
				break;
			}
		}

		LoadVertexProperties(vec3Pos, scale, size, &texCoords[0], rotation, vec4Color, texId);
	}

	void RenderBatch::LoadVertexProperties(const Transform& transform, const SpriteRenderer& spr)
	{
		glm::vec4 color = spr.m_Color;
		const Sprite& sprite = spr.m_Sprite;
		const glm::vec2* texCoords = spr.m_Sprite.m_TexCoords;
		glm::vec2 quadSize{ sprite.m_Width, sprite.m_Height };
		float rotation = transform.m_EulerRotation.z;

		int texId = 0;
		if (sprite.m_Texture != TextureHandle::null)
		{
			for (int i = 0; i < m_Textures.size(); i++)
			{
				if (m_Textures[i] == sprite.m_Texture)
				{
					texId = i + 1;
					break;
				}
			}
		}

		Entity res = Entity::FromComponent<Transform>(transform);
		LoadVertexProperties(transform.m_Position, transform.m_Scale, quadSize, texCoords, rotation, color, texId, res.GetID());
	}

	void RenderBatch::LoadVertexProperties(const glm::vec3& position, const glm::vec3& scale, const glm::vec2& quadSize, const glm::vec2* texCoords,
		float rotationDegrees, const glm::vec4& color, int texId, uint32 entityId)
	{
		bool isRotated = rotationDegrees != 0.0f;
		glm::mat4 matrix = glm::mat4(1.0f);
		if (isRotated)
		{
			matrix = glm::translate(matrix, position);
			matrix = glm::rotate(matrix, glm::radians(rotationDegrees), glm::vec3(0, 0, 1));
			matrix = glm::scale(matrix, scale * glm::vec3(quadSize.x, quadSize.y, 1));
		}

		float xAdd = 0.5f;
		float yAdd = -0.5f;
		for (int i = 0; i < 4; i++)
		{
			if (i == 1)
			{
				yAdd = 0.5f;
			}
			else if (i == 2)
			{
				xAdd = -0.5f;
			}
			else if (i == 3)
			{
				yAdd = -0.5f;
			}

			glm::vec4 currentPos = glm::vec4(position.x + (xAdd * scale.x * quadSize.x),
				position.y + (yAdd * scale.y * quadSize.y), 0.0f, 1.0f);
			if (isRotated)
			{
				currentPos = matrix * glm::vec4(xAdd, yAdd, 0.0f, 1.0f);
			}

			// Load Attributes
			m_VertexStackPointer->position = glm::vec3(currentPos);
			m_VertexStackPointer->color = glm::vec4(color);
			m_VertexStackPointer->texCoords = glm::vec2(texCoords[i]);
			m_VertexStackPointer->texId = (float)texId;
			m_VertexStackPointer->entityId = (entityId + 1);

			m_VertexStackPointer++;
		}
	}

	void RenderBatch::LoadVertexProperties(const glm::vec2* vertices, const glm::vec2* texCoords, const glm::vec4& color, int texId, uint32 entityId)
	{
		for (int i = 0; i < 4; i++)
		{
			// Load Attributes
			m_VertexStackPointer->position = glm::vec3(vertices[i].x, vertices[i].y, 0.0f);
			m_VertexStackPointer->color = glm::vec4(color);
			m_VertexStackPointer->texCoords = glm::vec2(texCoords[i]);
			m_VertexStackPointer->texId = (float)texId;
			m_VertexStackPointer->entityId = (entityId + 1);

			m_VertexStackPointer++;
		}
	}

	void RenderBatch::LoadEmptyVertexProperties()
	{
		for (int i = 0; i < 4; i++)
		{
			m_VertexStackPointer = {};
			m_VertexStackPointer++;
		}
	}

	void RenderBatch::Render()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 4 * m_NumSprites, &m_VertexBufferBase[0]);

		for (int i = 0; i < this->m_NumTextures; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i + 1);
			m_Textures[i].Get()->Bind();
		}

		glBindVertexArray(m_VAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glDrawElements(GL_TRIANGLES, m_NumSprites * 6, GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindVertexArray(0);

		for (int i = 0; i < this->m_NumTextures; i++)
		{
			m_Textures[i].Get()->Unbind();
		}
	}

	void RenderBatch::GenerateIndices()
	{
		for (int i = 0; i < m_MaxBatchSize; i++)
		{
			this->LoadElementIndices(i);
		}
	}

	void RenderBatch::LoadElementIndices(int index)
	{
		int offsetArray = 6 * index;
		int offset = 4 * index;

		// Triangle 1
		this->m_Indices[offsetArray] = offset + 3;
		this->m_Indices[offsetArray + 1] = offset + 2;
		this->m_Indices[offsetArray + 2] = offset + 0;

		// Triangle 2
		this->m_Indices[offsetArray + 3] = offset + 0;
		this->m_Indices[offsetArray + 4] = offset + 2;
		this->m_Indices[offsetArray + 5] = offset + 1;
	}

	void RenderBatch::Clear()
	{
		this->m_VertexStackPointer = m_VertexBufferBase;
		this->m_NumSprites = 0;
		this->m_NumTextures = 0;
		for (int i = 0; i < m_NumTextures; i++)
		{
			m_Textures[i] = TextureHandle::null;
		}
	}
}