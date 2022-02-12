#ifndef UTILS_H
#define UTILS_H

#include <string>
#ifndef WIN32
#include <libgen.h>
#include <unistd.h>
#include <linux/limits.h>
#else
#include <Windows.h>
#endif

class Utils
{
public:
    static std::string workingDirectory()
    {
        char workingDir[256];
#ifndef WIN32
        ssize_t count = readlink("/proc/self/exe", workingDir, PATH_MAX);
        if (count)
            return std::string(dirname(workingDir));
        return "";
#else
        GetModuleFileNameA(NULL, workingDir, 256);
        std::string::size_type pos = std::string(workingDir).find_last_of("\\/");
        return std::string(workingDir).substr(0, pos + 1);
#endif
    }
};

#endif