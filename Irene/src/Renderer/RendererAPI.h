#pragma once

#include "ipch.h"
#include "glm/glm.hpp"

#include "Renderer/VertexArray.h"

namespace IRENE {
	
	class RendererAPI {
	public:
		enum class API {
			None = 0,
			OpenGL = 1
		};

		virtual void Clear() = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;

		inline static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};
}