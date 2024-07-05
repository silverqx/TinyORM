#pragma once
#ifndef ORM_UTILS_TYPE_HPP
#define ORM_UTILS_TYPE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QString>

#include <typeindex>
#include <typeinfo>

#ifdef __GNUG__
#  include <cxxabi.h>
#endif

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

class QVariant; // clazy:exclude=qt6-fwd-fixes

TINYORM_BEGIN_COMMON_NAMESPACE

#if defined(__GNUG__) || defined(__clang__)
// NOLINTNEXTLINE(bugprone-reserved-identifier)
#  define __tiny_func__ \
    Orm::Utils::Type::prettyFunction(static_cast<const char *>(__PRETTY_FUNCTION__))
#elif defined(_MSC_VER)
// NOLINTNEXTLINE(bugprone-reserved-identifier)
#  define __tiny_func__ \
    Orm::Utils::Type::prettyFunction(static_cast<const char *>(__FUNCTION__))
#else
#  define __tiny_func__ QString::fromUtf8(__FUNCTION__)
#endif

namespace Orm::Utils
{

    /*! Types library class. */
    class TINYORM_EXPORT Type
    {
        Q_DISABLE_COPY_MOVE(Type)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        Type() = delete;
        /*! Deleted destructor. */
        ~Type() = delete;

        /*! Class name with or w/o a namespace and w/o template parameters. */
        template<typename T>
        static QString classPureBasename(bool withNamespace = false);
        /*! Class name with or w/o a namespace and w/o template parameters. */
        template<typename T>
        static QString
        classPureBasename(const T &type, bool withNamespace = false);
        /*! Class name with or w/o a namespace and w/o template parameters. */
        static QString
        classPureBasename(std::type_index typeIndex, bool withNamespace = false);

        /*! Return a pretty function name in the following format: Xyz::function. */
        static QString prettyFunction(const QString &function);
        /*! Return a pretty function name in the following format: Xyz::function. */
        inline static QString prettyFunction(const char *function);

        /*! Determine if a string is the true bool value (false for "", '0', "false",
            "off"). */
        static bool isTrue(const QString &value);
        /*! Determine if a string is the true bool value (false for "", '0', "false",
            "off"). */
        static bool isTrue(const QVariant &value);

        /*! Resolve/normalize CMake TriState bool value, internally call isCMakeTrue(),
            so return ON/OFF for Classic bool or NOTFOUND/xyz-NOTFOUND (to uppercase). */
        static QString normalizeCMakeTriStateBool(const QString &value);
        /*! Resolve/normalize CMake TriState bool value, internally call isCMakeTrue(),
            so return ON/OFF for the Classic bool. */
        static QString normalizeCMakeBool(const QString &value);
        /*! Determine if a string is the CMake true bool value (1, ON, YES, TRUE, Y, or
            a non-zero number (including floating point numbers). */
        static bool isCMakeTrue(const QString &value);

    private:
        /*! Class name with or w/o a namespace and w/o template parameters, common
            code. */
        static QString
        classPureBasenameInternal(const std::type_info &typeInfo, bool withNamespace);
        /*! Class name with or w/o a namespace and w/o template parameters, common
            code. */
        static QString
        classPureBasenameInternal(const char *typeName, bool withNamespace);
        /*! Class name with or w/o a namespace and w/o template parameters, MSVC code. */
        static QString
        classPureBasenameMsvc(const QString &className, bool withNamespace);
        /*! Class name with or w/o a namespace and w/o template parameters, GCC code. */
        static QString
        classPureBasenameGcc(const QString &className, bool withNamespace);
    };

    /* public */

    template<typename T>
    QString Type::classPureBasename(const bool withNamespace)
    {
        return classPureBasenameInternal(typeid (T), withNamespace);
    }

    template<typename T>
    QString Type::classPureBasename(const T &type, const bool withNamespace)
    {
        /* If you want to obtain a name for the polymorphic type, take care to pass
           a glvalue as the 'type' argument, the 'this' pointer is a prvalue!
           Above is whole true, but doesn't make sense as if I passing this or better
           *this to this function then it will be implicitly converted to the reference
           for polymorphic types as the parameter is const &.
           Anyway for polymorphic types pass *this! */
        return classPureBasenameInternal(typeid (type), withNamespace);
    }

    QString Type::prettyFunction(const char *const function)
    {
        return prettyFunction(QString::fromUtf8(function));
    }

} // namespace Orm::Utils

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_UTILS_TYPE_HPP
