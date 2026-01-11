#pragma once
enum class EditorMode { SELECTION, TRANSLATE, ROTATION, SCALE, PLACEMENT, DELETION, PAINT };
struct EditorState {
    bool showHierarchy = true;
    bool showInspector = true;
    bool showGrid = true;
    EditorMode currentMode = EditorMode::SELECTION;
};