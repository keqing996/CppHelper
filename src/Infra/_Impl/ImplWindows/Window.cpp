#include "../../Windows/Window.h"
#include "../../Windows/WindowsDefine.h"

#if PLATFORM_WINDOWS

#include "../../ScopeGuard.h"
#include "../../String.h"
#include "../../Logger.h"

namespace Infra
{
    class Support
    {
    public:
        Support() = delete;

    public:
        static bool FixProcessDpi()
        {
            // Try SetProcessDpiAwareness first
            if (FixProcessDpiBySetProcessDpiAwareness())
                return true;

            // Fall back to SetProcessDPIAware
            if (FixProcessDpiBySetProcessDPIAware())
                return true;

            return false;
        }

    private:
        static bool FixProcessDpiBySetProcessDpiAwareness()
        {
            HINSTANCE pShcodeDll = ::LoadLibraryW(L"Shcore.dll");

            if (pShcodeDll == nullptr)
                return false;

            ScopeGuard pShcodeDllReleaseGuard = [&pShcodeDll]()
            {
                ::FreeLibrary(pShcodeDll);
            };

            void* pFuncSetProcessDpiAwareness = reinterpret_cast<void*>(GetProcAddress(pShcodeDll, "SetProcessDpiAwareness"));
            if (pFuncSetProcessDpiAwareness == nullptr)
                return false;

            enum ProcessDpiAwareness
            {
                ProcessDpiUnaware         = 0,
                ProcessSystemDpiAware     = 1,
                ProcessPerMonitorDpiAware = 2
            };

            using SetProcessDpiAwarenessFuncType = HRESULT (WINAPI*)(ProcessDpiAwareness);

            auto setProcessDpiAwarenessFunc = reinterpret_cast<SetProcessDpiAwarenessFuncType>(pFuncSetProcessDpiAwareness);
            if (setProcessDpiAwarenessFunc(ProcessPerMonitorDpiAware) == E_INVALIDARG)
                return false;

            return true;
        }

        static bool FixProcessDpiBySetProcessDPIAware()
        {
            HINSTANCE pUser32Dll = ::LoadLibraryW(L"user32.dll");

            if (pUser32Dll == nullptr)
                return false;

            ScopeGuard pShcodeDllReleaseGuard = [&pUser32Dll]()
            {
                ::FreeLibrary(pUser32Dll);
            };

            void* pFuncSetProcessDPIAware = reinterpret_cast<void*>(GetProcAddress(pUser32Dll, "SetProcessDPIAware"));
            if (pFuncSetProcessDPIAware == nullptr)
                return false;

            using SetProcessDPIAwareFuncType = BOOL (WINAPI*)(void);

            auto setProcessDpiAwareFunc = reinterpret_cast<SetProcessDPIAwareFuncType>(pFuncSetProcessDPIAware);
            if (!setProcessDpiAwareFunc())
                return false;

            return true;
        }
    };




}

#endif