#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#include <shlobj.h> // For SHGetFolderPath
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

std::string getPath() {
    std::string homePath;

    #ifdef _WIN32
    // Windows systems
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, path))) {
        homePath = std::string(path);
    }
    #else
    // Unix systems
    if (const char* envHome = getenv("HOME")) {
        homePath = std::string(envHome);
    } else {
        // Fallback for Unix systems where HOME is not set
        struct passwd *pw = getpwuid(getuid());
        if (pw) {
            homePath = std::string(pw->pw_dir);
        }
    }
    #endif
    //use this syntax with your own dir to concatenate w/ homePath
    std::string pathToCSC3380 = homePath + "/VS_Code/"; // Riley & Don Need this
    //std::string pathToCSC3380 = homePath + "/github-classroom/hkaiserteaching/";
    return pathToCSC3380;
}