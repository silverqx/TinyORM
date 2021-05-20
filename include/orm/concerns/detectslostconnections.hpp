#ifndef DETECTSLOSTCONNECTIONS_HPP
#define DETECTSLOSTCONNECTIONS_HPP

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{
    class SqlError;

namespace Concerns
{

    class DetectsLostConnections
    {
        DetectsLostConnections(const DetectsLostConnections &) = delete;
        DetectsLostConnections &operator=(const DetectsLostConnections &) = delete;

    public:
        DetectsLostConnections() = default;

        /*! Determine if the given exception was caused by a lost connection. */
        bool causedByLostConnection(const SqlError &e) const;
    };

} // namespace Orm::Concerns
} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // DETECTSLOSTCONNECTIONS_HPP
