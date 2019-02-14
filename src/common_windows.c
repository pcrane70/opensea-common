//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2012 - 2018 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file common_windows.c
// \brief Implements functions that are common to MSFT Windows.
//
#include "common.h"
#include "common_windows.h"
#include <windows.h> //needed for all the stuff to get the windows version
#include <Strsafe.h> //needed in the code written to get the windows version since I'm using a Microsoft provided string concatenation call-tje
#include <IO.h> //needed for getting the size of a file in windows

bool os_Directory_Exists(const char * const pathToCheck)
{
    DWORD attrib = INVALID_FILE_ATTRIBUTES;
#if defined (UNICODE)
    //determine what the length of the new string will be
    size_t pathCheckLength = mbstowcs(NULL, pathToCheck, strlen(pathToCheck) + 1);
    //allocate memory
    LPWSTR localPathToCheckBuf = (LPWSTR)calloc(pathCheckLength + 2, sizeof(WCHAR));
    LPCWSTR localPathToCheck = &localPathToCheckBuf[0];
    //convert incoming string to work
    pathCheckLength = mbstowcs(localPathToCheckBuf, pathToCheck, strlen(pathToCheck) + 1);
#else
    LPCSTR localPathToCheck = (LPCSTR)pathToCheck;
#endif

    attrib = GetFileAttributes(localPathToCheck);

#if defined (UNICODE)
    safe_Free(localPathToCheckBuf);
    localPathToCheck = NULL;
#endif

    if (attrib == INVALID_FILE_ATTRIBUTES)
    {
        //printf("getlasterror = %d(0x%x)\n", GetLastError(), GetLastError());
        return false;
    }
    else if (attrib & FILE_ATTRIBUTE_DIRECTORY)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool os_File_Exists(const char * const filetoCheck)
{
    DWORD attrib = INVALID_FILE_ATTRIBUTES;
#if defined (UNICODE)
    //determine what the length of the new string will be
    size_t fileCheckLength = mbstowcs(NULL, filetoCheck, strlen(filetoCheck) + 1);
    //allocate memory
    LPWSTR localFileToCheckBuf = (LPWSTR)calloc(fileCheckLength + 2, sizeof(WCHAR));
    LPCWSTR localFileToCheck = &localFileToCheckBuf[0];
    //convert incoming string to work
    fileCheckLength = mbstowcs(localFileToCheckBuf, filetoCheck, strlen(filetoCheck) + 1);
#else
    LPCSTR localFileToCheck = (LPCSTR)filetoCheck;
#endif

    attrib = GetFileAttributes(localFileToCheck);

#if defined (UNICODE)
    safe_Free(localFileToCheckBuf);
    localFileToCheck = NULL;
#endif

    if (attrib == INVALID_FILE_ATTRIBUTES)
    {
        //printf("getlasterror = %d(0x%x)\n", GetLastError(), GetLastError());
        return false;
    }
    else if (attrib & FILE_ATTRIBUTE_DIRECTORY)
    {
        return false; //It's a directory, not a file. 
    }
    else
    {
        return true;
    }
}

int get_Full_Path(const char * pathAndFile, char fullPath[OPENSEA_PATH_MAX])
{
    if (!pathAndFile || !fullPath)
    {
        return BAD_PARAMETER;
    }
    //TODO:?Use macro to work with the wide charcter version?
    DWORD result = GetFullPathNameA(pathAndFile, OPENSEA_PATH_MAX, fullPath, NULL);
    if (result == 0)
    {
        //fatal error
        return FAILURE;
    }
    else if (result > OPENSEA_PATH_MAX)
    {
        //This case is when the function returns a buffer size that is required to hold the full path name.
        //DWORD lastError = GetLastError();
        //TODO: change things based on error code?
        return FAILURE;
    }

    return SUCCESS;
}

static uint16_t get_Console_Default_Color()
{
    static uint16_t defaultConsoleAttributes = UINT16_MAX;
    if (defaultConsoleAttributes == UINT16_MAX)
    {
        CONSOLE_SCREEN_BUFFER_INFO defaultInfo;
        memset(&defaultInfo, 0, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &defaultInfo))
        {
            defaultConsoleAttributes = defaultInfo.wAttributes;
        }
        else
        {
            //set defaultColorVal to zero
            defaultConsoleAttributes = 0;
        }
    }
    return defaultConsoleAttributes;
}

void set_Console_Colors(bool foregroundBackground, eConsoleColors consoleColor)
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD theColor = 0;
    if (foregroundBackground)//change foreground color
    {
        switch (consoleColor)
        {
        case DARK_BLUE:
            theColor = FOREGROUND_BLUE;
            break;
        case BLUE:
            theColor = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        case DARK_GREEN:
            theColor = FOREGROUND_GREEN;
            break;
        case GREEN:
            theColor = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case DARK_RED:
            theColor = FOREGROUND_RED;
            break;
        case RED:
            theColor = FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case BLACK:
            theColor = 0;//this should mean no colors or black
            break;
        case BROWN:
            theColor = FOREGROUND_RED | FOREGROUND_GREEN;
            break;
        case YELLOW:
            theColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case TEAL:
            theColor = FOREGROUND_BLUE | FOREGROUND_GREEN;
            break;
        case CYAN:
            theColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case PURPLE:
            theColor = FOREGROUND_BLUE | FOREGROUND_RED;
            break;
        case MAGENTA:
            theColor = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case WHITE:
            theColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case DARK_GRAY:
            theColor = FOREGROUND_INTENSITY;
            break;
        case GRAY:
            theColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
            break;
        case DEFAULT://fall through to default
        default:
            //theColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
            theColor = get_Console_Default_Color();
        }
        SetConsoleTextAttribute(consoleHandle, theColor);
    }
    else//change background color
    {
        switch (consoleColor)
        {
        case DARK_BLUE:
            theColor = BACKGROUND_BLUE;
            break;
        case BLUE:
            theColor = BACKGROUND_BLUE | BACKGROUND_INTENSITY;
            break;
        case DARK_GREEN:
            theColor = BACKGROUND_GREEN;
            break;
        case GREEN:
            theColor = BACKGROUND_GREEN | BACKGROUND_INTENSITY;
            break;
        case DARK_RED:
            theColor = BACKGROUND_RED;
            break;
        case RED:
            theColor = BACKGROUND_RED | BACKGROUND_INTENSITY;
            break;
        case BLACK:
            theColor = 0;//this should mean no colors or black
            break;
        case BROWN:
            theColor = BACKGROUND_RED | BACKGROUND_GREEN;
            break;
        case YELLOW:
            theColor = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
            break;
        case TEAL:
            theColor = BACKGROUND_BLUE | BACKGROUND_GREEN;
            break;
        case CYAN:
            theColor = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
            break;
        case PURPLE:
            theColor = BACKGROUND_BLUE | BACKGROUND_RED;
            break;
        case MAGENTA:
            theColor = BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY;
            break;
        case WHITE:
            theColor = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;
            break;
        case DARK_GRAY:
            theColor = BACKGROUND_INTENSITY;
            break;
        case GRAY:
            theColor = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
            break;
        case DEFAULT://fall through to default
        default:
            //theColor = 0;//black background
            //theColor |= FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;//white foreground
            theColor = get_Console_Default_Color();
        }
        SetConsoleTextAttribute(consoleHandle, theColor);
    }
}

eArchitecture get_Compiled_Architecture(void)
{
    //check which compiler we're using to use it's preprocessor definitions
    //intel compiler and visual c seem to use the same preprocessor definitions
    #if defined _MSC_VER || defined __INTEL_COMPILER
        #if defined _M_X64 || defined _M_AMD64
            return OPENSEA_ARCH_X86_64;
        #elif defined _M_ALPHA
            return OPENSEA_ARCH_ALPHA;
        #elif defined _M_ARM || defined _M_ARMT
            return OPENSEA_ARCH_ARM;
        #elif defined _M_IX86
            return OPENSEA_ARCH_X86;
        #elif defined _M_IA64
            return OPENSEA_ARCH_IA_64;
        #elif defined _M_PPC //32bits I'm guessing - TJE
            return OPENSEA_ARCH_POWERPC;
        #else
            return OPENSEA_ARCH_UNKNOWN;
        #endif
    #elif defined __MINGW32__ || defined __MINGW64__ || defined __clang__ || defined __GNUC__ //I'm guessing that all these compilers will use the same macro definitions since the sourceforge pages aren't 100% complete (clang I'm unsure about)
        #if defined __alpha__
            return OPENSEA_ARCH_ALPHA;
        #elif defined __amd64__ || defined __x86_64__
            return OPENSEA_ARCH_X86_64;
        #elif defined __arm__ || defined __thumb__
            return OPENSEA_ARCH_ARM;
        #elif defined __aarch64__
            return OPENSEA_ARCH_ARM64;
        #elif defined __i386__ || defined __i486__ || defined __i586__ || defined __i686__
            return OPENSEA_ARCH_X86;
        #elif defined __ia64__ || defined __IA64__
            return OPENSEA_ARCH_IA_64;
        #elif defined __powerpc64__ || defined __PPC64__ || defined __ppc64__ || defined _ARCH_PPC64
            return OPENSEA_ARCH_POWERPC64;
        #elif defined __powerpc__ || defined __POWERPC__ || defined __PPC__ || defined __ppc__ || defined _ARCH_PPC
            return OPENSEA_ARCH_POWERPC;
        #elif defined __sparc__
            return OPENSEA_ARCH_SPARC;
        #elif defined __MINGW32__ && defined _X86_
            return OPENSEA_ARCH_X86;
        #else
            return OPENSEA_ARCH_UNKNOWN;
        #endif
    #else
        return OPENSEA_ARCH_UNKNOWN;
    #endif
}
eEndianness calculate_Endianness(void)
{
    static eEndianness endian = OPENSEA_UNKNOWN_ENDIAN;//using static so that it should only need to run this code once...not that it takes a long time, but this may help optimise this.
    if (endian == OPENSEA_UNKNOWN_ENDIAN)
    {
        union
        {
            uint32_t value;
            uint8_t data[sizeof(uint32_t)];
        } number;

        number.data[0] = 0x00;
        number.data[1] = 0x01;
        number.data[2] = 0x02;
        number.data[3] = 0x03;

        switch (number.value)
        {
        case UINT32_C(0x00010203):
            endian = OPENSEA_BIG_ENDIAN;
            break;
        case UINT32_C(0x03020100):
            endian = OPENSEA_LITTLE_ENDIAN;
            break;
        case UINT32_C(0x02030001):
            endian = OPENSEA_BIG_WORD_ENDIAN;
            break;
        case UINT32_C(0x01000302):
            endian = OPENSEA_LITTLE_WORD_ENDIAN;
            break;
        default:
            endian = OPENSEA_UNKNOWN_ENDIAN;
            break;
        }
    }
    return endian;
}

//from what I can tell, windows basically only ever runs on little endian, but this complete check SHOULD MOSTLY work
eEndianness get_Compiled_Endianness(void)
{
    //check which compiler we're using to use it's preprocessor definitions
    //intel compiler and visual c seem to use the same preprocessor definitions
    #if defined _MSC_VER || defined __INTEL_COMPILER
        #if defined _M_X64 || defined _M_AMD64
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined _M_ALPHA
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined _M_ARM || defined _M_ARMT
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined _M_IX86
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined _M_IA64
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined _M_PPC //32bits I'm guessing - TJE
            return OPENSEA_BIG_ENDIAN;
        #elif defined __LITTLE_ENDIAN__
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined __BIG_ENDIAN__
            return OPENSEA_BIG_ENDIAN;
        #else
            return calculate_Endianness();
        #endif
    #elif defined __MINGW32__ || defined __MINGW64__ || defined __clang__ || defined __GNUC__ //I'm guessing that all these compilers will use the same macro definitions since the sourceforge pages aren't 100% complete (clang I'm unsure about)
        #if defined (__BYTE_ORDER__)
            #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
                return OPENSEA_BIG_ENDIAN;
            #elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
                return OPENSEA_LITTLE_ENDIAN;
            #else
                return calculate_Endianness();
            #endif
        #elif defined (__BIG_ENDIAN__)
            return OPENSEA_BIG_ENDIAN;
        #elif defined (__LITTLE_ENDIAN__)
            return OPENSEA_LITTLE_ENDIAN;
        #else
            //check architecture specific defines...
            #if defined (__ARMEB__) || defined (__THUMBEB__) || defined (__AARCH64EB__) || defined (_MIPSEB) || defined (__MIPSEB) || defined (__MIPSEB__)
                return OPENSEA_BIG_ENDIAN;
            #elif defined (__ARMEL__) || defined (__THUMBEL__) || defined (__AARCH64EL__) || defined (_MIPSEL) || defined (__MIPSEL) || defined (__MIPSEL__)
                return OPENSEA_LITTLE_ENDIAN;
            #else
                return calculate_Endianness();
            #endif
        #endif
    #endif
}

bool is_Windows_8_Or_Higher()
{
    bool isWindows8OrHigher = false;
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG conditionMask = 0;
    memset(&windowsVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    conditionMask = 0;
    //Now get the actual version of the OS...start at windows vista and work forward from there.
    windowsVersionInfo.dwMajorVersion = 6;
    windowsVersionInfo.dwMinorVersion = 2;
    conditionMask = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    if (VerifyVersionInfo(&windowsVersionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
    {
        isWindows8OrHigher = true;
    }
    return isWindows8OrHigher;
}

bool is_Windows_8_One_Or_Higher()
{
    bool isWindows81OrHigher = false;
    //Will only work if app manifested correctly
    /*
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG conditionMask = 0;
    memset(&windowsVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    conditionMask = 0;
    //Now get the actual version of the OS...start at windows vista and work forward from there.
    windowsVersionInfo.dwMajorVersion = 6;
    windowsVersionInfo.dwMinorVersion = 3;
    conditionMask = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    if (VerifyVersionInfo(&windowsVersionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
    {
        isWindows81OrHigher = true;
    }
    /*/
    //This uses our method below that checks the version from kernel.dll
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion > 6)
    {
        //Win10 or higher
        isWindows81OrHigher = true;
    }
    else if (windowsVersion.versionType.windowsVersion.majorVersion == 6 && windowsVersion.versionType.windowsVersion.minorVersion >= 3)
    {
        isWindows81OrHigher = true;
    }
    //*/
    return isWindows81OrHigher;
}

bool is_Windows_10_Or_Higher()
{
    bool isWindows10OrHigher = false;
    //Will only work if app manifested correctly
    /*
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG conditionMask = 0;
    memset(&windowsVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    conditionMask = 0;
    //Now get the actual version of the OS...start at windows vista and work forward from there.
    windowsVersionInfo.dwMajorVersion = 10;
    windowsVersionInfo.dwMinorVersion = 0;
    conditionMask = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    if (VerifyVersionInfo(&windowsVersionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
    {
        isWindows10OrHigher = true;
    }
    /*/
    //This uses our method below that checks the version from kernel.dll
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10)
    {
        //Win10 or higher
        isWindows10OrHigher = true;
    }
    //*/
    return isWindows10OrHigher;
}

bool is_Windows_Server_OS()
{
    bool isWindowsServer = false;
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG conditionMask = 0;
    memset(&windowsVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    windowsVersionInfo.wProductType = VER_NT_WORKSTATION;
    conditionMask = VerSetConditionMask(conditionMask, VER_PRODUCT_TYPE, VER_EQUAL);//checking for equality on the workstation attribute
    if (VerifyVersionInfo(&windowsVersionInfo, VER_PRODUCT_TYPE, conditionMask))
    {
        //Windows workstation/desktop
        isWindowsServer = false;
    }
    else
    {
        //Windows server/domain controller (which is a windows server version)
        isWindowsServer = true;
    }
    return isWindowsServer;
}

//possible MSDN api calls: 
//https://msdn.microsoft.com/en-us/library/windows/desktop/aa370624(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/hardware/ff561910(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms724451(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms725492(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms724358(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms647003(v=vs.85).aspx //Used this one since it is supposed to work!
//From MSDN: To obtain the full version number for the operating system, call the GetFileVersionInfo function on one of the system DLLs, such as Kernel32.dll, then call VerQueryValue to obtain the \\StringFileInfo\\<lang><codepage>\\ProductVersion subblock of the file version information.
int get_Operating_System_Version_And_Name(ptrOSVersionNumber versionNumber, char *operatingSystemName)
{
    int ret = SUCCESS;
    bool isWindowsServer = is_Windows_Server_OS();
    memset(versionNumber, 0, sizeof(OSVersionNumber));
    versionNumber->osVersioningIdentifier = OS_WINDOWS;
    //start getting the Windows version using the verifyVersionInfo call. I'm doing it this way since the "version helpers" are essentially doing the same thing but are not available to minGW
    
#if defined (UNICODE)
    static const wchar_t kernel32DLL[] = L"\\kernel32.dll";
    LPWSTR systemPathBuf = (LPWSTR)calloc(OPENSEA_PATH_MAX, sizeof(WCHAR));
    LPCWSTR systemPath = &systemPathBuf[0];
    LPCWSTR subblock = L"\\";
#else
    static const char kernel32DLL[] = "\\kernel32.dll";
    LPSTR *systemPathBuf = (LPSTR)calloc(OPENSEA_PATH_MAX, sizeof(CHAR));
    LPCSTR systemPath = &systemPathBuf[0];
    LPCSTR subblock = "\\";
#endif

    if(!systemPath)
    {
        return MEMORY_FAILURE;
    }

    UINT directoryStringLength = GetSystemDirectory(systemPathBuf, OPENSEA_PATH_MAX);
    if (directoryStringLength > OPENSEA_PATH_MAX || directoryStringLength == 0 || directoryStringLength > OPENSEA_PATH_MAX - sizeof(kernel32DLL) / sizeof(*kernel32DLL))
    {
        //error
        safe_Free(systemPathBuf);
        systemPath = NULL;
        return FAILURE;
    }
    //I'm using this Microsoft provided call to concatenate strings since it will concatenate properly for ansi or wide strings depending on whether UNICODE is set or not - TJE
    if (S_OK != StringCchCat(systemPathBuf, OPENSEA_PATH_MAX, kernel32DLL))
    {
        return FAILURE;
    }

    DWORD versionInfoSize = GetFileVersionInfoSize(systemPath, NULL);
    if (versionInfoSize > 0)
    {
        LPVOID ver = malloc(versionInfoSize);
        if (!ver)
        {
            safe_Free(systemPathBuf);
            systemPath = NULL;
            return MEMORY_FAILURE;
        }
        if (GetFileVersionInfo(systemPathBuf, 0, versionInfoSize, ver))
        {
            LPVOID block = NULL;
            UINT blockSize = sizeof(VS_FIXEDFILEINFO);//start with this size...should get at least this
            VS_FIXEDFILEINFO *versionFileInfo = NULL;
            if (VerQueryValue((LPCVOID)ver, subblock, &block, &blockSize) || blockSize < sizeof(VS_FIXEDFILEINFO))//this should run the first function before performing the comparison
            {
                versionFileInfo = (VS_FIXEDFILEINFO*)block;
                versionNumber->versionType.windowsVersion.majorVersion = HIWORD(versionFileInfo->dwProductVersionMS);
                versionNumber->versionType.windowsVersion.minorVersion = LOWORD(versionFileInfo->dwProductVersionMS);
                versionNumber->versionType.windowsVersion.buildNumber = HIWORD(versionFileInfo->dwProductVersionLS);
            }
            else
            {
                //error
                ret = FAILURE;
            }
        }
        else
        {
            //error
            ret = FAILURE;
        }
        safe_Free(ver);
    }
    else
    {
        //error
        ret = FAILURE;
    }
    safe_Free(systemPathBuf);
    systemPath = NULL;

    if (ret == SUCCESS)
    {
        //Now that we know whether or not it's a server version and have gotten the version number, set the appropriate string for the OS.
        if (operatingSystemName)
        {
            switch (versionNumber->versionType.windowsVersion.majorVersion)
            {
            case 10://Win 10 or Server 2016
                switch (versionNumber->versionType.windowsVersion.minorVersion)
                {
                case 0:
                    if (isWindowsServer)
                    {
                        sprintf(&operatingSystemName[0], "Windows Server 2016");
                    }
                    else
                    {
                        sprintf(&operatingSystemName[0], "Windows 10");
                    }
                    break;
                default:
                    if (isWindowsServer)
                    {
                        sprintf(&operatingSystemName[0], "Windows Server 2016 or higher");
                    }
                    else
                    {
                        sprintf(&operatingSystemName[0], "Windows 10 or higher");
                    }
                    break;
                }
                break;
            case 6://Vista through 8.1
                switch (versionNumber->versionType.windowsVersion.minorVersion)
                {
                case 3:
                    if (isWindowsServer)
                    {
                        sprintf(&operatingSystemName[0], "Windows Server 2012 R2");
                    }
                    else
                    {
                        sprintf(&operatingSystemName[0], "Windows 8.1");
                    }
                    break;
                case 2:
                    if (isWindowsServer)
                    {
                        sprintf(&operatingSystemName[0], "Windows Server 2012");
                    }
                    else
                    {
                        sprintf(&operatingSystemName[0], "Windows 8");
                    }
                    break;
                case 1:
                    if (isWindowsServer)
                    {
                        sprintf(&operatingSystemName[0], "Windows Server 2008 R2");
                    }
                    else
                    {
                        sprintf(&operatingSystemName[0], "Windows 7");
                    }
                    break;
                case 0:
                    if (isWindowsServer)
                    {
                        sprintf(&operatingSystemName[0], "Windows Server 2008");
                    }
                    else
                    {
                        sprintf(&operatingSystemName[0], "Windows Vista");
                    }
                    break;
                default:
                    if (isWindowsServer)
                    {
                        sprintf(&operatingSystemName[0], "Windows Server 2012 R2 or higher");
                    }
                    else
                    {
                        sprintf(&operatingSystemName[0], "Windows 8.1 or higher");
                    }
                    break;
                }
                break;
            case 5://2000 through XP
                switch (versionNumber->versionType.windowsVersion.minorVersion)
                {
                case 2:
                    if (isWindowsServer)
                    {
                        sprintf(&operatingSystemName[0], "Windows Server 2003");
                    }
                    else
                    {
                        sprintf(&operatingSystemName[0], "Windows XP 64-Bit Edition");
                    }
                    break;
                case 1:
                    sprintf(&operatingSystemName[0], "Windows XP");
                    break;
                case 0:
                    sprintf(&operatingSystemName[0], "Windows 2000");
                    break;
                default:
                    if (isWindowsServer)
                    {
                        sprintf(&operatingSystemName[0], "Windows Server 2003 or higher");
                    }
                    else
                    {
                        sprintf(&operatingSystemName[0], "XP or higher");
                    }
                    break;
                }
                break;
            default:
                sprintf(&operatingSystemName[0], "Unknown Windows OS");
                break;
            }
        }
    }
    return ret;
}

int64_t os_Get_File_Size(FILE *filePtr)
{
    LARGE_INTEGER fileSize;
    //convert the fileptr to a HANDLE
    HANDLE fileHandle = (HANDLE)_get_osfhandle(_fileno(filePtr));
    fileSize.QuadPart = 0;//set to something before we call GetFileSizeEx
    //use GetFileSizeEx to get the size of the file as a 64bit integer
    if (GetFileSizeEx(fileHandle, &fileSize))
    {
        return fileSize.QuadPart;
    }
    else
    {
        return -1;
    }
}

void start_Timer(seatimer_t *timer)
{
    LARGE_INTEGER tempLargeInt;
    tempLargeInt.QuadPart = 0;
    if (QueryPerformanceCounter(&tempLargeInt))//according to MSDN this will always return success on XP and later systems
    {
        timer->timerStart = tempLargeInt.QuadPart;
    }
}

void stop_Timer(seatimer_t *timer)
{
    LARGE_INTEGER tempLargeInt;
    memset(&tempLargeInt, 0, sizeof(LARGE_INTEGER));
    if (QueryPerformanceCounter(&tempLargeInt))//according to MSDN this will always return success on XP and later systems
    {
        timer->timerStop = tempLargeInt.QuadPart;
    }
}

uint64_t get_Nano_Seconds(seatimer_t timer)
{
    LARGE_INTEGER frequency;//clock ticks per second
    uint64_t ticksPerNanosecond = 1000000000;//start with a count of nanoseconds per second
    uint64_t seconds = 0, nanoSeconds = 0;
    memset(&frequency, 0, sizeof(LARGE_INTEGER));
    QueryPerformanceFrequency(&frequency);//should always return success
    ticksPerNanosecond /= frequency.QuadPart;
    seconds = (timer.timerStop - timer.timerStart) / frequency.QuadPart;//converted to nanoseconds later
    nanoSeconds = ((timer.timerStop - timer.timerStart) % frequency.QuadPart) * ticksPerNanosecond;
    return ((seconds * 1000000000) + nanoSeconds);
}

double get_Micro_Seconds(seatimer_t timer)
{
    uint64_t nanoseconds = get_Nano_Seconds(timer);
    return ((double)nanoseconds / 1000.00);
}

double get_Milli_Seconds(seatimer_t timer)
{
    return (get_Micro_Seconds(timer) / 1000.00);
}

double get_Seconds(seatimer_t timer)
{
    return (get_Milli_Seconds(timer) / 1000.00);
}

void print_Windows_Error_To_Screen(unsigned int windowsError)
{
    LPSTR windowsErrorString = NULL;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, windowsError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&windowsErrorString, 0, NULL);
    printf("%u - %s\n", windowsError, windowsErrorString);
    LocalFree(windowsErrorString);
}