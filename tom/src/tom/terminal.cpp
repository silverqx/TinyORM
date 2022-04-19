#include "tom/terminal.hpp"

#ifdef _WIN32
#  include <qt_windows.h>
#elif defined(__linux__)
#  include <sys/ioctl.h>
#endif

#include <io.h>

#include <orm/utils/type.hpp>

#include "tom/exceptions/invalidargumenterror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{

/* public */

// I can tell that ansi logic detection in this class is a real porn 😎

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
    // CUR tom, linux silverqx
    return _isatty(_fileno(stream)) != 0;
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
    // CUR tom, finish linux silverqx
    struct winsize w;
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
#endif

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE
