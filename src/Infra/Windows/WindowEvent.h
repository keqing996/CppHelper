#pragma once

#include "../PlatformDefine.h"

#if PLATFORM_WINDOWS

namespace Infra
{
    struct WindowSizeEvent
    {
        unsigned int width;
        unsigned int height;
    };

    struct KeyEvent
    {
        Keyboard::Key code;          //!< Code of the key that has been pressed
        Keyboard::Scancode scancode; //!< Physical code of the key that has been pressed
        bool          alt;           //!< Is the Alt key pressed?
        bool          control;       //!< Is the Control key pressed?
        bool          shift;         //!< Is the Shift key pressed?
        bool          system;        //!< Is the System key pressed?
    };


}

#endif