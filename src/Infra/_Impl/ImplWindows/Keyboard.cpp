#include "../../Windows/Window.h"
#include "../../Windows/WindowsDefine.h"

#if PLATFORM_WINDOWS

#include "../../Windows/Keyboard.h"

namespace Infra
{
    bool Keyboard::IsKeyPressed(Key key)
    {
        int virtualKey = GetWinVirtualKey(key);

        // GetAsyncKeyState() specifies whether the key was pressed
        // since the last call to GetAsyncKeyState(), and whether the
        // key is currently up or down. If the most significant bit
        // is set, the key is down, and if the least significant bit
        // is set, the key was pressed after the previous call
        // to GetAsyncKeyState().
        return (::GetAsyncKeyState(virtualKey) & 0x8000) != 0;
    }

    int Keyboard::GetWinVirtualKey(Key key)
    {
        switch (key)
        {
            case Key::A:                return 'A';
            case Key::B:                return 'B';
            case Key::C:                return 'C';
            case Key::D:                return 'D';
            case Key::E:                return 'E';
            case Key::F:                return 'F';
            case Key::G:                return 'G';
            case Key::H:                return 'H';
            case Key::I:                return 'I';
            case Key::J:                return 'J';
            case Key::K:                return 'K';
            case Key::L:                return 'L';
            case Key::M:                return 'M';
            case Key::N:                return 'N';
            case Key::O:                return 'O';
            case Key::P:                return 'P';
            case Key::Q:                return 'Q';
            case Key::R:                return 'R';
            case Key::S:                return 'S';
            case Key::T:                return 'T';
            case Key::U:                return 'U';
            case Key::V:                return 'V';
            case Key::W:                return 'W';
            case Key::X:                return 'X';
            case Key::Y:                return 'Y';
            case Key::Z:                return 'Z';
            case Key::Num0:             return '0';
            case Key::Num1:             return '1';
            case Key::Num2:             return '2';
            case Key::Num3:             return '3';
            case Key::Num4:             return '4';
            case Key::Num5:             return '5';
            case Key::Num6:             return '6';
            case Key::Num7:             return '7';
            case Key::Num8:             return '8';
            case Key::Num9:             return '9';
            case Key::Escape:           return VK_ESCAPE;
            case Key::LControl:         return VK_LCONTROL;
            case Key::LShift:           return VK_LSHIFT;
            case Key::LAlt:             return VK_LMENU;
            case Key::LSystem:          return VK_LWIN;
            case Key::RControl:         return VK_RCONTROL;
            case Key::RShift:           return VK_RSHIFT;
            case Key::RAlt:             return VK_RMENU;
            case Key::RSystem:          return VK_RWIN;
            case Key::Menu:             return VK_APPS;
            case Key::LBracket:         return VK_OEM_4;
            case Key::RBracket:         return VK_OEM_6;
            case Key::Semicolon:        return VK_OEM_1;
            case Key::Comma:            return VK_OEM_COMMA;
            case Key::Period:           return VK_OEM_PERIOD;
            case Key::Apostrophe:       return VK_OEM_7;
            case Key::Slash:            return VK_OEM_2;
            case Key::Backslash:        return VK_OEM_5;
            case Key::Grave:            return VK_OEM_3;
            case Key::Equal:            return VK_OEM_PLUS;
            case Key::Minus:            return VK_OEM_MINUS;
            case Key::Space:            return VK_SPACE;
            case Key::Enter:            return VK_RETURN;
            case Key::Backspace:        return VK_BACK;
            case Key::Tab:              return VK_TAB;
            case Key::PageUp:           return VK_PRIOR;
            case Key::PageDown:         return VK_NEXT;
            case Key::End:              return VK_END;
            case Key::Home:             return VK_HOME;
            case Key::Insert:           return VK_INSERT;
            case Key::Delete:           return VK_DELETE;
            case Key::NumpadAdd:        return VK_ADD;
            case Key::NumPadMinus:      return VK_SUBTRACT;
            case Key::NumPadMultiply:   return VK_MULTIPLY;
            case Key::NumPadDivide:     return VK_DIVIDE;
            case Key::ArrowLeft:        return VK_LEFT;
            case Key::ArrowRight:       return VK_RIGHT;
            case Key::ArrowUp:          return VK_UP;
            case Key::ArrowDown:        return VK_DOWN;
            case Key::NumPad0:          return VK_NUMPAD0;
            case Key::NumPad1:          return VK_NUMPAD1;
            case Key::NumPad2:          return VK_NUMPAD2;
            case Key::NumPad3:          return VK_NUMPAD3;
            case Key::NumPad4:          return VK_NUMPAD4;
            case Key::NumPad5:          return VK_NUMPAD5;
            case Key::NumPad6:          return VK_NUMPAD6;
            case Key::NumPad7:          return VK_NUMPAD7;
            case Key::NumPad8:          return VK_NUMPAD8;
            case Key::NumPad9:          return VK_NUMPAD9;
            case Key::F1:               return VK_F1;
            case Key::F2:               return VK_F2;
            case Key::F3:               return VK_F3;
            case Key::F4:               return VK_F4;
            case Key::F5:               return VK_F5;
            case Key::F6:               return VK_F6;
            case Key::F7:               return VK_F7;
            case Key::F8:               return VK_F8;
            case Key::F9:               return VK_F9;
            case Key::F10:              return VK_F10;
            case Key::F11:              return VK_F11;
            case Key::F12:              return VK_F12;
            case Key::F13:              return VK_F13;
            case Key::F14:              return VK_F14;
            case Key::F15:              return VK_F15;
            case Key::Pause:            return VK_PAUSE;
            default:                    return 0;
        }
    }


}

#endif