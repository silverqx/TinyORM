#ifndef CONNECTIONRESOLVERINTERFACE_H
#define CONNECTIONRESOLVERINTERFACE_H

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{
    class DatabaseConnection;

    class ConnectionResolverInterface
    {
    public:
        // TODO next virtual dtor in abstract class multi ingeritance silverqx
        virtual ~ConnectionResolverInterface() = default;

        /*! Get a database connection instance. */
        virtual DatabaseConnection &connection(const QString &name = "") = 0;

        /*! Get the default connection name. */
        virtual const QString &getDefaultConnection() const = 0;

        /*! Set the default connection name. */
        virtual void setDefaultConnection(const QString &defaultConnection) = 0;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // CONNECTIONRESOLVERINTERFACE_H
