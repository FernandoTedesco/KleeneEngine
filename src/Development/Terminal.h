class Terminal{

    public:
    Terminal();
    void Clear();
    void WriteArt();
    ~Terminal();
    private:
    void* windowHandle;
    void* outputHandle;
    

    
};