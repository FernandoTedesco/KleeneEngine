#include <string>
class Editor;

enum LogCategory {
    LOG_NONE = 0,
    LOG_INFO = 1 << 0,
    LOG_WARNING = 1 << 1,
    LOG_ERROR = 1 << 2,
    LOG_SUCCESS = 1 << 3,

    LOG_RENDER = 1 << 4,
    LOG_IO = 1 << 6,
    LOG_CORE = 1 << 7,

    LOG_ALL = 0xFFFF
};
class Terminal
{

public:
    static void Log(LogCategory type, const std::string& message);
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
    static unsigned int ActiveFilters;
};