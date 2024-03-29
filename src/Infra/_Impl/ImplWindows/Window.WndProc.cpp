#include "../../Windows/Window.h"
#include "../../Windows/WindowsDefine.h"

#if PLATFORM_WINDOWS

#include "../../ScopeGuard.h"
#include "../../String.h"
#include "../../Logger.h"

namespace Infra
{
    void Window::WindowEventProcess(uint32_t message, void* wpara, void* lpara)
    {
        if (_hWindow == nullptr)
            return;

        WPARAM wParam = reinterpret_cast<WPARAM>(wpara);
        LPARAM lParam = reinterpret_cast<LPARAM>(lpara);

        switch (message)
        {
        // Destroy event
        case WM_DESTROY:
            {
                // Here we must cleanup resources !
                cleanup();
                break;
            }

            // Set cursor event
        case WM_SETCURSOR:
            {
                // The mouse has moved, if the cursor is in our window we must refresh the cursor
                if (LOWORD(lParam) == HTCLIENT) {
                    SetCursor(m_cursorVisible ? m_lastCursor : NULL);
                }

                break;
            }

            // Close event
        case WM_CLOSE:
            {
                Event event;
                event.type = Event::Closed;
                pushEvent(event);
                break;
            }

            // Resize event
        case WM_SIZE:
            {
                // Consider only events triggered by a maximize or a un-maximize
                if (wParam != SIZE_MINIMIZED && !m_resizing && m_lastSize != getSize())
                {
                    // Update the last handled size
                    m_lastSize = getSize();

                    // Push a resize event
                    Event event;
                    event.type        = Event::Resized;
                    event.size.width  = m_lastSize.x;
                    event.size.height = m_lastSize.y;
                    pushEvent(event);

                    // Restore/update cursor grabbing
                    grabCursor(m_cursorGrabbed);
                }
                break;
            }

            // Start resizing
        case WM_ENTERSIZEMOVE:
            {
                m_resizing = true;
                grabCursor(false);
                break;
            }

            // Stop resizing
        case WM_EXITSIZEMOVE:
            {
                m_resizing = false;

                // Ignore cases where the window has only been moved
                if(m_lastSize != getSize())
                {
                    // Update the last handled size
                    m_lastSize = getSize();

                    // Push a resize event
                    Event event;
                    event.type        = Event::Resized;
                    event.size.width  = m_lastSize.x;
                    event.size.height = m_lastSize.y;
                    pushEvent(event);
                }

                // Restore/update cursor grabbing
                grabCursor(m_cursorGrabbed);
                break;
            }

            // The system request the min/max window size and position
        case WM_GETMINMAXINFO:
            {
                // We override the returned information to remove the default limit
                // (the OS doesn't allow windows bigger than the desktop by default)
                MINMAXINFO* info = reinterpret_cast<MINMAXINFO*>(lParam);
                info->ptMaxTrackSize.x = 50000;
                info->ptMaxTrackSize.y = 50000;
                break;
            }

            // Gain focus event
        case WM_SETFOCUS:
            {
                // Restore cursor grabbing
                grabCursor(m_cursorGrabbed);

                Event event;
                event.type = Event::GainedFocus;
                pushEvent(event);
                break;
            }

            // Lost focus event
        case WM_KILLFOCUS:
            {
                // Ungrab the cursor
                grabCursor(false);

                Event event;
                event.type = Event::LostFocus;
                pushEvent(event);
                break;
            }

            // Text event
        case WM_CHAR:
            {
                if (m_keyRepeatEnabled || ((lParam & (1 << 30)) == 0))
                {
                    // Get the code of the typed character
                    Uint32 character = static_cast<Uint32>(wParam);

                    // Check if it is the first part of a surrogate pair, or a regular character
                    if ((character >= 0xD800) && (character <= 0xDBFF))
                    {
                        // First part of a surrogate pair: store it and wait for the second one
                        m_surrogate = static_cast<Uint16>(character);
                    }
                    else
                    {
                        // Check if it is the second part of a surrogate pair, or a regular character
                        if ((character >= 0xDC00) && (character <= 0xDFFF))
                        {
                            // Convert the UTF-16 surrogate pair to a single UTF-32 value
                            Uint16 utf16[] = {m_surrogate, static_cast<Uint16>(character)};
                            sf::Utf16::toUtf32(utf16, utf16 + 2, &character);
                            m_surrogate = 0;
                        }

                        // Send a TextEntered event
                        Event event;
                        event.type = Event::TextEntered;
                        event.text.unicode = character;
                        pushEvent(event);
                    }
                }
                break;
            }

            // Keydown event
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            {
                if (m_keyRepeatEnabled || ((HIWORD(lParam) & KF_REPEAT) == 0))
                {
                    Event event;
                    event.type         = Event::KeyPressed;
                    event.key.alt      = HIWORD(GetKeyState(VK_MENU))    != 0;
                    event.key.control  = HIWORD(GetKeyState(VK_CONTROL)) != 0;
                    event.key.shift    = HIWORD(GetKeyState(VK_SHIFT))   != 0;
                    event.key.system   = HIWORD(GetKeyState(VK_LWIN)) || HIWORD(GetKeyState(VK_RWIN));
                    event.key.code     = virtualKeyCodeToSF(wParam, lParam);
                    event.key.scancode = toScancode(wParam, lParam);
                    pushEvent(event);
                }
                break;
            }

            // Keyup event
        case WM_KEYUP:
        case WM_SYSKEYUP:
            {
                Event event;
                event.type         = Event::KeyReleased;
                event.key.alt      = HIWORD(GetKeyState(VK_MENU))    != 0;
                event.key.control  = HIWORD(GetKeyState(VK_CONTROL)) != 0;
                event.key.shift    = HIWORD(GetKeyState(VK_SHIFT))   != 0;
                event.key.system   = HIWORD(GetKeyState(VK_LWIN)) || HIWORD(GetKeyState(VK_RWIN));
                event.key.code     = virtualKeyCodeToSF(wParam, lParam);
                event.key.scancode = toScancode(wParam, lParam);
                pushEvent(event);
                break;
            }

            // Vertical mouse wheel event
        case WM_MOUSEWHEEL:
            {
                // Mouse position is in screen coordinates, convert it to window coordinates
                POINT position;
                position.x = static_cast<Int16>(LOWORD(lParam));
                position.y = static_cast<Int16>(HIWORD(lParam));
                ScreenToClient(m_handle, &position);

                Int16 delta = static_cast<Int16>(HIWORD(wParam));

                Event event;

                event.type             = Event::MouseWheelMoved;
                event.mouseWheel.delta = delta / 120;
                event.mouseWheel.x     = position.x;
                event.mouseWheel.y     = position.y;
                pushEvent(event);

                event.type                   = Event::MouseWheelScrolled;
                event.mouseWheelScroll.wheel = Mouse::VerticalWheel;
                event.mouseWheelScroll.delta = static_cast<float>(delta) / 120.f;
                event.mouseWheelScroll.x     = position.x;
                event.mouseWheelScroll.y     = position.y;
                pushEvent(event);
                break;
            }

            // Horizontal mouse wheel event
        case WM_MOUSEHWHEEL:
            {
                // Mouse position is in screen coordinates, convert it to window coordinates
                POINT position;
                position.x = static_cast<Int16>(LOWORD(lParam));
                position.y = static_cast<Int16>(HIWORD(lParam));
                ScreenToClient(m_handle, &position);

                Int16 delta = static_cast<Int16>(HIWORD(wParam));

                Event event;
                event.type                   = Event::MouseWheelScrolled;
                event.mouseWheelScroll.wheel = Mouse::HorizontalWheel;
                event.mouseWheelScroll.delta = -static_cast<float>(delta) / 120.f;
                event.mouseWheelScroll.x     = position.x;
                event.mouseWheelScroll.y     = position.y;
                pushEvent(event);
                break;
            }

            // Mouse left button down event
        case WM_LBUTTONDOWN:
            {
                Event event;
                event.type               = Event::MouseButtonPressed;
                event.mouseButton.button = Mouse::Left;
                event.mouseButton.x      = static_cast<Int16>(LOWORD(lParam));
                event.mouseButton.y      = static_cast<Int16>(HIWORD(lParam));
                pushEvent(event);
                break;
            }

            // Mouse left button up event
        case WM_LBUTTONUP:
            {
                Event event;
                event.type               = Event::MouseButtonReleased;
                event.mouseButton.button = Mouse::Left;
                event.mouseButton.x      = static_cast<Int16>(LOWORD(lParam));
                event.mouseButton.y      = static_cast<Int16>(HIWORD(lParam));
                pushEvent(event);
                break;
            }

            // Mouse right button down event
        case WM_RBUTTONDOWN:
            {
                Event event;
                event.type               = Event::MouseButtonPressed;
                event.mouseButton.button = Mouse::Right;
                event.mouseButton.x      = static_cast<Int16>(LOWORD(lParam));
                event.mouseButton.y      = static_cast<Int16>(HIWORD(lParam));
                pushEvent(event);
                break;
            }

            // Mouse right button up event
        case WM_RBUTTONUP:
            {
                Event event;
                event.type               = Event::MouseButtonReleased;
                event.mouseButton.button = Mouse::Right;
                event.mouseButton.x      = static_cast<Int16>(LOWORD(lParam));
                event.mouseButton.y      = static_cast<Int16>(HIWORD(lParam));
                pushEvent(event);
                break;
            }

            // Mouse wheel button down event
        case WM_MBUTTONDOWN:
            {
                Event event;
                event.type               = Event::MouseButtonPressed;
                event.mouseButton.button = Mouse::Middle;
                event.mouseButton.x      = static_cast<Int16>(LOWORD(lParam));
                event.mouseButton.y      = static_cast<Int16>(HIWORD(lParam));
                pushEvent(event);
                break;
            }

            // Mouse wheel button up event
        case WM_MBUTTONUP:
            {
                Event event;
                event.type               = Event::MouseButtonReleased;
                event.mouseButton.button = Mouse::Middle;
                event.mouseButton.x      = static_cast<Int16>(LOWORD(lParam));
                event.mouseButton.y      = static_cast<Int16>(HIWORD(lParam));
                pushEvent(event);
                break;
            }

            // Mouse X button down event
        case WM_XBUTTONDOWN:
            {
                Event event;
                event.type               = Event::MouseButtonPressed;
                event.mouseButton.button = HIWORD(wParam) == XBUTTON1 ? Mouse::XButton1 : Mouse::XButton2;
                event.mouseButton.x      = static_cast<Int16>(LOWORD(lParam));
                event.mouseButton.y      = static_cast<Int16>(HIWORD(lParam));
                pushEvent(event);
                break;
            }

            // Mouse X button up event
        case WM_XBUTTONUP:
            {
                Event event;
                event.type               = Event::MouseButtonReleased;
                event.mouseButton.button = HIWORD(wParam) == XBUTTON1 ? Mouse::XButton1 : Mouse::XButton2;
                event.mouseButton.x      = static_cast<Int16>(LOWORD(lParam));
                event.mouseButton.y      = static_cast<Int16>(HIWORD(lParam));
                pushEvent(event);
                break;
            }

            // Mouse leave event
        case WM_MOUSELEAVE:
            {
                // Avoid this firing a second time in case the cursor is dragged outside
                if (m_mouseInside)
                {
                    m_mouseInside = false;

                    // Generate a MouseLeft event
                    Event event;
                    event.type = Event::MouseLeft;
                    pushEvent(event);
                }
                break;
            }

            // Mouse move event
        case WM_MOUSEMOVE:
            {
                // Extract the mouse local coordinates
                int x = static_cast<Int16>(LOWORD(lParam));
                int y = static_cast<Int16>(HIWORD(lParam));

                // Get the client area of the window
                RECT area;
                ::GetClientRect(m_handle, &area);

                // Capture the mouse in case the user wants to drag it outside
                if ((wParam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON | MK_XBUTTON1 | MK_XBUTTON2)) == 0)
                {
                    // Only release the capture if we really have it
                    if (::GetCapture() == m_handle)
                        ::ReleaseCapture();
                }
                else if (::GetCapture() != m_handle)
                {
                    // Set the capture to continue receiving mouse events
                    ::SetCapture(m_handle);
                }

                // If the cursor is outside the client area...
                if ((x < area.left) || (x > area.right) || (y < area.top) || (y > area.bottom))
                {
                    // and it used to be inside, the mouse left it.
                    if (m_mouseInside)
                    {
                        m_mouseInside = false;

                        // No longer care for the mouse leaving the window
                        setTracking(false);

                        // Generate a MouseLeft event
                        Event event;
                        event.type = Event::MouseLeft;
                        pushEvent(event);
                    }
                }
                else
                {
                    // and vice-versa
                    if (!m_mouseInside)
                    {
                        m_mouseInside = true;

                        // Look for the mouse leaving the window
                        setTracking(true);

                        // Generate a MouseEntered event
                        Event event;
                        event.type = Event::MouseEntered;
                        pushEvent(event);
                    }
                }

                // Generate a MouseMove event
                Event event;
                event.type        = Event::MouseMoved;
                event.mouseMove.x = x;
                event.mouseMove.y = y;
                pushEvent(event);
                break;
            }

            // Hardware configuration change event
        case WM_DEVICECHANGE:
            {
                // Some sort of device change has happened, update joystick connections
                if ((wParam == DBT_DEVICEARRIVAL) || (wParam == DBT_DEVICEREMOVECOMPLETE))
                {
                    // Some sort of device change has happened, update joystick connections if it is a device interface
                    DEV_BROADCAST_HDR* deviceBroadcastHeader = reinterpret_cast<DEV_BROADCAST_HDR*>(lParam);

                    if (deviceBroadcastHeader && (deviceBroadcastHeader->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE))
                        JoystickImpl::updateConnections();
                }

                break;
            }
        }

    }
}