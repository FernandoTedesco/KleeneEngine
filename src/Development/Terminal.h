#include <string>
class Editor;
class Terminal
{

public:
    Terminal();
    void Clear();
    void WriteArt();
    void Help();

    unsigned long UpdateConsoleInput();
    void ProcessConsoleInput();
    void ExecuteConsoleCommand();
    void ChangeLog();
    void SetEditorContext(Editor* editor);
    ~Terminal();

private:
    void* windowHandle;
    void* outputHandle;
    void* inputHandle;
    Editor* editorContext = nullptr;
    std::string commandBuffer;
};