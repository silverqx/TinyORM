#include "tom/terminal.hpp"

#ifdef _WIN32
#  include <qt_windows.h>

#  include <fcntl.h>
#  include <io.h>
#elif defined(__linux__)
#  include <sys/ioctl.h>
#endif

#include <orm/utils/type.hpp>

#include "tom/exceptions/invalidargumenterror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{

/* public */

// I can tell that ansi logic detection in this class is a real porn 😎

void Terminal::initialize()
{
#ifdef _WIN32
    enableUtf8ConsoleEncoding();
#endif

#ifdef __MINGW32__
    enableVt100Support();
#endif
}

bool Terminal::hasColorSupport(std::ostream &cout) const
{
    auto *const coutPointer = std::addressof(cout);

    // Return from the cache, compute only once
    if (m_isAnsiOutput.contains(coutPointer))
        return m_isAnsiOutput[coutPointer];

    // Map c++ stream to the c stream, needed by the isatty()
    FILE *stream = nullptr;

    if (coutPointer == std::addressof(std::cout))
        stream = stdout;
    else if (coutPointer == std::addressof(std::cerr))
        stream = stderr;
    else
        throw Exceptions::InvalidArgumentError(
                QLatin1String("Unsupported stream type passed in %1().")
                .arg(__tiny_func__));

    // Autodetect
    const auto isAnsi = hasColorSupportInternal(cout, stream);

    // Cache the result
    m_isAnsiOutput.emplace(coutPointer, isAnsi);

    return isAnsi;
}

bool Terminal::hasWColorSupport(std::wostream &wcout) const
{
    auto *const wcoutPointer = std::addressof(wcout);

    // Return from the cache, compute only once
    if (m_isAnsiWOutput.contains(wcoutPointer))
        return m_isAnsiWOutput[wcoutPointer];

    // Map c++ stream to the c stream, needed by the isatty()
    FILE *stream = nullptr;

    if (wcoutPointer == std::addressof(std::wcout))
        stream = stdout;
    else if (wcoutPointer == std::addressof(std::wcerr))
        stream = stderr;
    else
        throw Exceptions::InvalidArgumentError(
                QLatin1String("Unsupported stream type passed in %1().")
                .arg(__tiny_func__));

    // Autodetect
    const auto isAnsi = hasColorSupportInternal(wcout, stream);

    // Cache the result
    m_isAnsiWOutput.emplace(wcoutPointer, isAnsi);

    return isAnsi;
}

bool Terminal::isatty(FILE *stream) const
{
#ifdef _WIN32
    return ::_isatty(::_fileno(stream)) != 0;
#else
    return ::isatty(::fileno(stream)) != 0;
#endif
}

int Terminal::width()
{
    if (const auto widthRaw = qEnvironmentVariable("COLUMNS");
        !widthRaw.isEmpty()
    ) {
        auto ok = false;
        const auto width = widthRaw.toInt(&ok);

        if (ok)
            return m_lastWidth = width;
    }

    if (auto [width, _] = terminalSize(); width != -1)
        return m_lastWidth = width;

    return m_lastWidth;
}

int Terminal::height()
{
    if (const auto heightRaw = qEnvironmentVariable("LINES");
        !heightRaw.isEmpty()
    ) {
        auto ok = false;
        const auto height = heightRaw.toInt(&ok);

        if (ok)
            return m_lastHeight = height;
    }

    if (auto [_, height] = terminalSize(); height != -1)
        return m_lastHeight = height;

    return m_lastHeight;
}

Terminal::TerminalSize Terminal::terminalSize() const
{
    int width  = -1;
    int height = -1;

#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    width  = static_cast<int>(csbi.srWindow.Right - csbi.srWindow.Left) + 1;
    height = static_cast<int>(csbi.srWindow.Bottom - csbi.srWindow.Top) + 1;
#elif defined(__linux__)
    struct winsize w {};
    ioctl(fileno(stdout), TIOCGWINSZ, &w);

    width  = static_cast<int>(w.ws_col);
    height = static_cast<int>(w.ws_row);
#endif

    return {width, height};
}

/* private */

#ifdef _WIN32
namespace
{
    /*! Get Windows output handle by passed c++ output stream. */
    HANDLE getStdHandleByCppStream(std::ostream &cout = std::cout)
    {
        HANDLE handle = nullptr;

        if (std::addressof(cout) == std::addressof(std::cout))
            handle = GetStdHandle(STD_OUTPUT_HANDLE);
        else if (std::addressof(cout) == std::addressof(std::cerr))
            handle = GetStdHandle(STD_ERROR_HANDLE);
        else
            throw Exceptions::InvalidArgumentError(
                    QLatin1String("Unsupported stream type passed in %1().")
                    .arg(__tiny_func__));

        return handle;
    }

    /*! Get Windows output handle by passed c++ output stream, wide version. */
    HANDLE getStdHandleByCppStream(std::wostream &wcout = std::wcout)
    {
        HANDLE handle = nullptr;

        if (std::addressof(wcout) == std::addressof(std::wcout))
            handle = GetStdHandle(STD_OUTPUT_HANDLE);
        else if (std::addressof(wcout) == std::addressof(std::wcerr))
            handle = GetStdHandle(STD_ERROR_HANDLE);
        else
            throw Exceptions::InvalidArgumentError(
                    QLatin1String("Unsupported stream type passed in %1().")
                    .arg(__tiny_func__));

        return handle;
    }
} // namespace

bool Terminal::hasVt100Support(std::ostream &cout) const
{
    DWORD mode = 0;

    if (GetConsoleMode(getStdHandleByCppStream(cout), &mode) == 0)
        return false;

    return (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) ==
            ENABLE_VIRTUAL_TERMINAL_PROCESSING;
}

bool Terminal::hasVt100Support(std::wostream &wcout) const
{
    DWORD mode = 0;

    if (GetConsoleMode(getStdHandleByCppStream(wcout), &mode) == 0)
        return false;

    return (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) ==
            ENABLE_VIRTUAL_TERMINAL_PROCESSING;
}

/* Terminal initialization */

void Terminal::enableUtf8ConsoleEncoding()
{
    // Set it here so the user doesn't have to deal with this
    SetConsoleOutputCP(CP_UTF8);

    /* UTF-8 encoding is corrupted for narrow input functions, needed to use wcin/wstring
       for an input, input will be in the unicode encoding then needed to translate
       unicode to utf8, eg. by QString::fromStdWString(), WideCharToMultiByte(), or
       std::codecvt(). It also works with msys2 ucrt64 gcc/clang. */
    SetConsoleCP(CP_UTF8);
    _setmode(_fileno(stdin), _O_WTEXT);
}
#endif

#ifdef __MINGW32__
void Terminal::enableVt100Support()
{
    /* The vt100 is disabled by default on MSYS2 so have to be explicitly enabled:
       https://github.com/msys2/msys2-runtime/issues/91 */
    DWORD mode = 0;
    auto *const stdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(stdOutHandle, &mode);
    SetConsoleMode(stdOutHandle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    mode = 0;
    auto *const stdErrHandle = GetStdHandle(STD_ERROR_HANDLE);
    GetConsoleMode(stdErrHandle, &mode);
    SetConsoleMode(stdErrHandle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#endif

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE
