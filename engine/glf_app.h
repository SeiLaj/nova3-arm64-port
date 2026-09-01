#pragma once
#include <cstdint>

namespace glf {
struct CreationSettings {
    int width;
    int height;
    int flags;
};

class App {
public:
    App();
    virtual ~App();
    virtual bool Init(CreationSettings& s);
    virtual bool MyInit();
    virtual void Run();
    virtual void Update();
    virtual void Deinit();
    static App* GetInstance();
    static void SetInstance(App* a);
private:
    static App* s_instance;
    bool m_initialized;
    bool m_running;
};
} // namespace glf
