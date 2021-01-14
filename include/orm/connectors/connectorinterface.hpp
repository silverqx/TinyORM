#ifndef CONNECTORINTERFACE_HPP
#define CONNECTORINTERFACE_HPP

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Connectors
{
    using ConnectionName = QString;

    class ConnectorInterface
    {
    public:
        virtual ~ConnectorInterface() = default;

        /*! Establish a database connection. */
        virtual ConnectionName connect(const QVariantHash &config) const = 0;
    };

} // namespace Orm::Connectors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // CONNECTORINTERFACE_HPP
