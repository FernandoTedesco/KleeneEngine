#pragma once
#include <functional>
#include <string>
#include "imgui.h"
#include "Development/EditorState.h"
#include "Development/EditorResources.h"

class ToolBar
{
public:
    ToolBar() = default;
    void SetIcons(const EditorIcons& icons);
    void Draw(float windowWidth, std::function<void(const char*)> onSaveRequest,
	      std::function<void(const char*)> onLoadRequest, EditorState& state);

private:
    void* iconSelection = nullptr;
    void* iconTranslate = nullptr;
    void* iconPlacement = nullptr;
    void* iconRotation = nullptr;
    void* iconScale = nullptr;
    void* iconDelete = nullptr;
    void* iconPaint = nullptr;
    bool DrawModesButton(void* iconID, bool isActive);
    void CheckKeys(EditorState& state);
    char saveBuffer[64] = "File";
    char loadBuffer[64] = "File";

    bool showSaveModal = false;
    bool showLoadModal = false;

    void DrawSaveModal(std::function<void(const char*)> onConfirmSave);
    void DrawLoadModal(std::function<void(const char*)> onConfirmLoad);
    void Stylize();
};