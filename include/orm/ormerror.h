#ifndef ORMERROR_H
#define ORMERROR_H

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    // TODO investigate and rework all orm exception classes silverqx
    class OrmError final : public std::runtime_error
    {
    public:
        explicit inline OrmError(const char *Message)
            : std::runtime_error(Message)
        {}
        explicit inline OrmError(const QString &Message)
            : std::runtime_error(Message.toUtf8().constData())
        {}
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // ORMERROR_H
