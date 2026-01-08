#pragma once
#include <functional>
#include <string>
#include "imgui.h"

class ToolBar
{
public:
    ToolBar() = default;

    void Draw(float windowWidth, std::function<void(const char*)> onSaveRequest,
	      std::function<void(const char*)> onLoadRequest);

private:
    char saveBuffer[64] = "File";
    char loadBuffer[64] = "File";

    bool showSaveModal = false;
    bool showLoadModal = false;

    void DrawSaveModal(std::function<void(const char*)> onConfirmSave);
    void DrawLoadModal(std::function<void(const char*)> onConfirmLoad);
};