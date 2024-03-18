#pragma once
#ifndef TOM_CONCERNS_CONFIRMABLE_HPP
#define TOM_CONCERNS_CONFIRMABLE_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <functional>

#include <orm/macros/commonnamespace.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
namespace Commands
{
    class Command;
}

namespace Concerns
{

    /*! Prints alert and asks for the confirmation (Y/N). */
    class Confirmable
    {
        Q_DISABLE_COPY(Confirmable)

    public:
        /*! Default constructor. */
        Confirmable() = default;
        /*! Pure virtual destructor. */
        inline virtual ~Confirmable() = 0;

        /*! Confirm before proceeding with the action (only in production environment). */
        bool confirmToProceed(
                    const QString &warning = QStringLiteral("Application In Production!"),
                    const std::function<bool()> &callback = nullptr) const;

    private:
        /*! Get the default confirmation callback. */
        std::function<bool()> defaultConfirmCallback() const;

        /*! Dynamic cast *this to the Command & base type, const version. */
        const Commands::Command &command() const;
    };

    /* public */

    Confirmable::~Confirmable() = default;

} // namespace Concerns
} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_CONCERNS_CONFIRMABLE_HPP
