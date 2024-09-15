#include "tom/terminal.hpp"

#ifdef _WIN32
#  include <qt_windows.h>

#  include <fcntl.h>
#  include <io.h>
#elif defined(__linux__)
#  include <sys/ioctl.h>
#endif

#ifndef _WIN32
#  include <unistd.h>
#endif

#include <orm/utils/type.hpp>

#include "tom/exceptions/invalidargumenterror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using Qt::StringLiterals::operator""_s;

namespace Tom
{

// I can tell that ANSI logic detection in this class is a real porn 😎

/* public */

/* Static operations on the Terminal class */

void Terminal::initialize()
{
#ifdef _WIN32
    enableUtf8ConsoleEncoding();
#endif

#ifdef __MINGW32__
    enableVt100Support();
#endif
}

bool Terminal::isatty(FILE *stream) noexcept
{
#ifdef _WIN32
    return ::_isatty(::_fileno(stream)) != 0;
#else
    return ::isatty(::fileno(stream)) != 0;
#endif
}

Terminal::TerminalSize Terminal::terminalSize() noexcept
{
    // Don't change to short, int is MUCH easier to manage, helps to avoid static_cast<>-s
    int width  = -1;
    int height = -1;

#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    width  = (csbi.srWindow.Right - csbi.srWindow.Left) + 1;
    height = (csbi.srWindow.Bottom - csbi.srWindow.Top) + 1;
#elif defined(__linux__)
    winsize w {};
    ioctl(fileno(stdout), TIOCGWINSZ, &w); // NOLINT(cppcoreguidelines-pro-type-vararg)

    width  = w.ws_col;
    height = w.ws_row;
#endif

    return {width, height};
}

/* Operations on a Terminal instance */

bool Terminal::hasColorSupport(const std::ostream &cout) const
{
    const auto *const coutPointer = std::addressof(cout);

    // Return from the cache, compute only once
    if (m_isAnsiOutput.contains(coutPointer))
        return m_isAnsiOutput.at(coutPointer);

    // Map c++ stream to the c stream, needed by the isatty()
    FILE *stream = nullptr;

    if (coutPointer == std::addressof(std::cout))
        stream = stdout;
    else if (coutPointer == std::addressof(std::cerr))
        stream = stderr;
    else
        throw Exceptions::InvalidArgumentError(
                u"Unsupported stream type passed in %1()."_s
                .arg(__tiny_func__));

    // Autodetect
    const auto isAnsi = hasColorSupportInternal(cout, stream);

    // Cache the result
    m_isAnsiOutput.emplace(coutPointer, isAnsi);

    return isAnsi;
}

bool Terminal::hasWColorSupport(const std::wostream &wcout) const
{
    const auto *const wcoutPointer = std::addressof(wcout);

    // Return from the cache, compute only once
    if (m_isAnsiWOutput.contains(wcoutPointer))
        return m_isAnsiWOutput.at(wcoutPointer);

    // Map c++ stream to the c stream, needed by the isatty()
    FILE *stream = nullptr;

    if (wcoutPointer == std::addressof(std::wcout))
        stream = stdout;
    else if (wcoutPointer == std::addressof(std::wcerr))
        stream = stderr;
    else
        throw Exceptions::InvalidArgumentError(
                u"Unsupported stream type passed in %1()."_s
                .arg(__tiny_func__));

    // Autodetect
    const auto isAnsi = hasColorSupportInternal(wcout, stream);

    // Cache the result
    m_isAnsiWOutput.emplace(wcoutPointer, isAnsi);

    return isAnsi;
}

int Terminal::width()
{
    if (const auto widthRaw = qEnvironmentVariable("COLUMNS");
        !widthRaw.isEmpty()
    ) {
        auto ok = false;
        const auto width = widthRaw.toInt(&ok);

        if (ok && width > 0)
            return m_lastWidth = width;
    }

    if (auto [width, _] = terminalSize(); width > 0)
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

        if (ok && height > 0)
            return m_lastHeight = height;
    }

    if (auto [_, height] = terminalSize(); height > 0)
        return m_lastHeight = height;

    return m_lastHeight;
}

/* private */

#ifdef _WIN32
namespace
{
    /*! Get Windows output handle by passed c++ output stream. */
    HANDLE getStdHandleByCppStream(const std::ostream &cout = std::cout)
    {
        HANDLE handle = nullptr;

        if (std::addressof(cout) == std::addressof(std::cout))
            handle = GetStdHandle(STD_OUTPUT_HANDLE);
        else if (std::addressof(cout) == std::addressof(std::cerr))
            handle = GetStdHandle(STD_ERROR_HANDLE);
        else
            throw Exceptions::InvalidArgumentError(
                    u"Unsupported stream type passed in %1()."_s
                    .arg(__tiny_func__));

        return handle;
    }

    /*! Get Windows output handle by passed c++ output stream, wide version. */
    HANDLE getStdHandleByCppStream(const std::wostream &wcout = std::wcout)
    {
        HANDLE handle = nullptr;

        if (std::addressof(wcout) == std::addressof(std::wcout))
            handle = GetStdHandle(STD_OUTPUT_HANDLE);
        else if (std::addressof(wcout) == std::addressof(std::wcerr))
            handle = GetStdHandle(STD_ERROR_HANDLE);
        else
            throw Exceptions::InvalidArgumentError(
                    u"Unsupported stream type passed in %1()."_s
                    .arg(__tiny_func__));

        return handle;
    }
} // namespace

bool Terminal::hasVt100Support(const std::ostream &cout)
{
    DWORD mode = 0;

    if (GetConsoleMode(getStdHandleByCppStream(cout), &mode) == 0)
        return false;

    return (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) ==
            ENABLE_VIRTUAL_TERMINAL_PROCESSING;
}

bool Terminal::hasVt100Support(const std::wostream &wcout)
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
       std::codecvt(). It also works with MSYS2 UCRT64 GCC/Clang. */
    SetConsoleCP(CP_UTF8);
    std::ignore = _setmode(_fileno(stdin), _O_WTEXT); // std::ignore to suppress C6031
}
#endif

#ifdef __MINGW32__
void Terminal::enableVt100Support()
{
    /* The VT100 is disabled by default on MSYS2 so have to be explicitly enabled:
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
