#include "Module.h"
#include <stdexcept>

ModuleLoader::ModuleLoader()
    : libs()
{

}

ModuleLoader::~ModuleLoader()
{
    for(auto it : libs)
        unload(it.first);
}

void ModuleLoader::load(const std::string& name)
{
    void* handle = dlopen(std::string("./lib" + name + ".so").c_str(), RTLD_LAZY);
    if(handle == nullptr)
        throw std::runtime_error(
            "could not load module " + name + ", error was:\n" + std::string(dlerror())
        );
    auto res = libs.insert(
        std::make_pair(name, handle)
    );
    if(res.second == false)
        throw std::runtime_error("module " + name + " already loaded");
    // call the load function
    ModuleLoad fun = reinterpret_cast<ModuleLoad>(dlsym(handle, "load"));
    (*fun)();
}

void ModuleLoader::unload(const std::string& name)
{
    auto lib = libs.find(name);
    if(lib == libs.end())
        throw std::runtime_error("module " + name + " cannot be unloaded for it was not loaded");
    void* handle = lib->second;
    ModuleUnload fun = reinterpret_cast<ModuleLoad>(dlsym(handle, "unload"));
    (*fun)();
    dlclose(handle);
}

void ModuleLoader::call(const std::string& lib, const std::string& sub, Data::XStack& stack,
                         SharedData& d)
{
    auto handle = libs.find(lib);
    if(handle == libs.end())
        throw std::runtime_error(sub + " cannot be called for " + lib + " was not loaded");
    ModuleCall fun = reinterpret_cast<ModuleCall>(dlsym(handle->second, "call"));
    (*fun)(sub.c_str(), stack, d);
}
