#pragma once
#ifndef ORM_CONCERNS_PARSESSEARCHPATH_HPP
#define ORM_CONCERNS_PARSESSEARCHPATH_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QStringList>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

class QVariant; // clazy:exclude=qt6-fwd-fixes

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Concerns
{

    /*! Parse the PostgreSQL search_path configuration value into the vector. */
    class TINYORM_EXPORT ParsesSearchPath
    {
        Q_DISABLE_COPY_MOVE(ParsesSearchPath)

    public:
        /*! Pure virtual destructor, to pass -Weffc++. */
        inline virtual ~ParsesSearchPath() = 0;

        /*! Determine whether the search_path is empty or contains ''. */
        inline static bool isSearchPathEmpty(const QString &searchPath);
        /*! Determine whether the search_path is empty or contains ''. */
        static bool isSearchPathEmpty(const QStringList &searchPath);

    protected:
        /*! Protected Default constructor. */
        ParsesSearchPath() = default;

        /*! Parse the PostgreSQL search_path configuration value into the vector. */
        static QStringList parseSearchPath(const QString &searchPath);
        /*! Parse the PostgreSQL search_path configuration value into the vector. */
        static QStringList parseSearchPath(const QStringList &searchPath);
        /*! Parse the PostgreSQL search_path configuration value into the vector. */
        static QStringList parseSearchPath(const QVariant &searchPath);
    };

    /* public */

    ParsesSearchPath::~ParsesSearchPath() = default;

    bool ParsesSearchPath::isSearchPathEmpty(const QString &searchPath)
    {
        return isSearchPathEmpty(QStringList {searchPath});
    }

} // namespace Orm::Concerns

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONCERNS_PARSESSEARCHPATH_HPP
