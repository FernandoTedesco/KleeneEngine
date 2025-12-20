
class Window;

class Editor{

    public:
    Editor(Window* window);
    void BeginFrame();
    void DrawEditorUI();
    void EndFrame();
    ~Editor();

    private:
    Window* window;
};