#include <string>

class Terminal{

    public:
    Terminal();
    void Clear();
    void WriteArt();
    void Help();

    unsigned long UpdateConsoleInput();
    void ProcessConsoleInput();
    void ExecuteConsoleCommand();
    ~Terminal();
    private:
    void* windowHandle;
    void* outputHandle;
    void *inputHandle;
    std::string commandBuffer;

    
};