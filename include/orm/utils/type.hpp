#pragma once
#ifndef UTILS_TYPE_H
#define UTILS_TYPE_H

#include <QString>

#include <typeindex>
#include <typeinfo>

#ifdef __GNUG__
#include <cxxabi.h>
#endif

#include "orm/utils/export.hpp"

#ifdef __GNUG__
#  define __tiny_func__ Orm::Utils::Type::prettyFunction(__PRETTY_FUNCTION__)
#elif _MSC_VER
#  define __tiny_func__ Orm::Utils::Type::prettyFunction(__FUNCTION__)
#else
#  define __tiny_func__ __FUNCTION__
#endif

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Utils
{

    /*! Library class for types. */
    class SHAREDLIB_EXPORT Type
    {
        Q_DISABLE_COPY(Type)

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
        static QString classPureBasename(const T &type, bool withNamespace = false);
        /*! Class name with or w/o a namespace and w/o template parameters. */
        static QString classPureBasename(
                std::type_index typeIndex, bool withNamespace = false);

        /*! Return a pretty function name in the following format: Xyz::function. */
        static QString prettyFunction(const QString &function);

    private:
        /*! Class name with or w/o a namespace and w/o template parameters, common
            code. */
        static QString classPureBasenameInternal(
                const std::type_info &typeInfo, bool withNamespace);
        /*! Class name with or w/o a namespace and w/o template parameters, common
            code. */
        static QString classPureBasenameInternal(
                const char *typeName, bool withNamespace);
        /*! Class name with or w/o a namespace and w/o template parameters, msvc code. */
        static QString classPureBasenameMsvc(
                const QString &className, bool withNamespace);
        /*! Class name with or w/o a namespace and w/o template parameters, gcc code. */
        static QString classPureBasenameGcc(
                const QString &className, bool withNamespace);
    };

    template<typename T>
    inline QString
    Type::classPureBasename(const bool withNamespace)
    {
        return classPureBasenameInternal(typeid (T), withNamespace);
    }

    template<typename T>
    inline QString
    Type::classPureBasename(const T &type, const bool withNamespace)
    {
        /* If you want to obtain a name for the polymorphic type, take care to pass
            a glvalue as the 'type' argument, the 'this' pointer is a prvalue! */
        return classPureBasenameInternal(typeid (type), withNamespace);
    }

} // namespace Orm::Utils
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // UTILS_TYPE_H
