#pragma once
#ifndef DETECTSLOSTCONNECTIONS_HPP
#define DETECTSLOSTCONNECTIONS_HPP

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

namespace Exceptions
{
    class SqlError;
}

namespace Concerns
{

    /*! Detect lost connection by passed exception message. */
    class DetectsLostConnections
    {
        /*! Deleted copy constructor. */
        DetectsLostConnections(const DetectsLostConnections &) = delete;
        /*! Deleted copy assignment operator. */
        DetectsLostConnections &operator=(const DetectsLostConnections &) = delete;

    public:
        /*! Default constructor. */
        DetectsLostConnections() = default;

        /*! Determine if the given exception was caused by a lost connection. */
        bool causedByLostConnection(const Exceptions::SqlError &e) const;
    };

} // namespace Orm::Concerns
} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // DETECTSLOSTCONNECTIONS_HPP
