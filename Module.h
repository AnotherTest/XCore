#ifndef _X_MODULE_H_INCLUDE_GUARD
#define _X_MODULE_H_INCLUDE_GUARD

#include <dlfcn.h>
#include <map>
#include "Data.h"

class ModuleLoader;
class SharedData;
typedef bool (*ModuleLoad)();
typedef bool (*ModuleUnload)();
typedef void (*ModuleCall)(const char*, Data::XStack&, SharedData&);

class ModuleLoader {
    std::map<std::string, void*> libs;
public:
    ModuleLoader();
    ~ModuleLoader();
    void load(const std::string& name);
    void unload(const std::string& name);
    void call(const std::string& lib, const std::string& sub, Data::XStack& stack, SharedData& d);
};

struct SharedData {
    Data::SubTable& routines;
    Data::Subroutine* current;
    ModuleLoader& modules;
    SharedData(Data::SubTable& subs, Data::Subroutine* curr, ModuleLoader& mods)
        : routines(subs), current(curr), modules(mods) {}
};

#endif // _X_MODULE_H_INCLUDE_GUARD

