#ifndef UTILS_TYPE_H
#define UTILS_TYPE_H

#include <QString>

#include <typeinfo>

#ifdef __GNUG__
#include <cxxabi.h>
#endif

// CUR move to orm/ silverqx
#include "export.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Utils
{

    /*! Library class for types. */
    class SHAREDLIB_EXPORT Type
    {
        Type(const Type &) = delete;
        Type &operator=(const Type &) = delete;

    public:
        Type() = delete;

        /*! Class name with or w/o a namespace and template parameters. */
        template<typename T>
        static QString classPureBasename(bool withNamespace = false);
        /*! Class name with or w/o a namespace and template parameters. */
        template<typename T>
        static QString classPureBasename(const T &type, bool withNamespace = false);

    private:
        /*! Class name with or w/o a namespace and template parameters, common code. */
        static QString classPureBasenameInternal(
                const std::type_info &typeInfo, bool withNamespace = false);
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
