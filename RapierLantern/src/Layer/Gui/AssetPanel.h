#pragma once

#include "Layer/Gui/Panel.h"

namespace Rapier {

	class AssetPanel : Panel {
	public:
		AssetPanel(Ref<Scene> scene)
			: Panel(scene) {}

		virtual void OnImGuiRender() override;

	private:
		void DrawTexturePanel();
		void DrawShaderPanel();

	};
}