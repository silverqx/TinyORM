#pragma once
#ifndef TOM_SEEDER_HPP
#define TOM_SEEDER_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <orm/db.hpp>
#include <orm/utils/nullvariant.hpp>

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom
{
    class Seeder;

namespace Concerns
{
    class InteractsWithIO;
}

    /*! Concept for the derived Seeder class. */
    template<typename T, typename ...Args>
    concept SeederConcept = std::derived_from<T, Seeder>;

    /*! Seeders base class. */
    class SHAREDLIB_EXPORT Seeder
    {
        Q_DISABLE_COPY(Seeder)

        /*! Alias for the InteractsWithIO. */
        using InteractsWithIO = Concerns::InteractsWithIO;

    public:
        /*! Default constructor. */
        inline Seeder() = default;
        /*! Pure virtual destructor. */
        inline virtual ~Seeder() = 0;

        /*! Seed the application's database. */
        inline virtual void run();

        /*! Call operator overload. */
        inline void operator()();

        /*! Run the given seeder classes. */
        template<SeederConcept ...T, typename ...Args>
        void call(bool silent = false, Args &&...args);
        /*! Run the given seeder classes, const version. */
        template<SeederConcept ...T, typename ...Args>
        void call(bool silent = false, Args &&...args) const;

        /*! Run the given seeder classes. */
        template<SeederConcept ...T, typename ...Args>
        void callWith(Args &&...args);
        /*! Run the given seeder classes, const version. */
        template<SeederConcept ...T, typename ...Args>
        void callWith(Args &&...args) const;

        /*! Silently run the given seeder classes. */
        template<SeederConcept ...T, typename ...Args>
        void callSilent(Args &&...args);
        /*! Silently run the given seeder classes, const version. */
        template<SeederConcept ...T, typename ...Args>
        void callSilent(Args &&...args) const;

        /*! Set the console input/ouput. */
        Seeder &setIO(const InteractsWithIO &io);

    private:
        /*! Run the given seeder classes. */
        template<typename ...Args>
        void callUnfolded(bool silent = false, Args &&...args);
        /*! Run the given seeder classes, const version. */
        template<typename ...Args>
        void callUnfolded(bool silent = false, Args &&...args) const;

        /*! Run the given seeder classes captured in the callback (helps to avoid
            duplicates). */
        void callInternal(bool silent, std::function<void()> &&callback) const;

        /*! Reference to the IO. */
        std::optional<
                std::reference_wrapper<const InteractsWithIO>> m_io = std::nullopt;
    };

    /* public */

    Seeder::~Seeder() = default;

    void Seeder::run()
    {}

    void Seeder::operator()()
    {
        run();
    }

    template<SeederConcept ...T, typename ...Args>
    void Seeder::call(const bool silent, Args &&...args)
    {
        (T().setIO(m_io.value())
            .callUnfolded(silent, std::forward<Args>(args)...), ...);
    }

    template<SeederConcept ...T, typename ...Args>
    void Seeder::call(const bool silent, Args &&...args) const
    {
        (std::add_const_t<T>().setIO(m_io.value())
                              .callUnfolded(silent, std::forward<Args>(args)...), ...);
    }

    template<SeederConcept ...T, typename ...Args>
    void Seeder::callWith(Args &&...args)
    {
        call(false, std::forward<Args>(args)...);
    }

    template<SeederConcept ...T, typename ...Args>
    void Seeder::callWith(Args &&...args) const
    {
        call(false, std::forward<Args>(args)...);
    }

    template<SeederConcept ...T, typename ...Args>
    void Seeder::callSilent(Args &&...args)
    {
        call(true, std::forward<Args>(args)...);
    }

    template<SeederConcept ...T, typename ...Args>
    void Seeder::callSilent(Args &&...args) const
    {
        call(true, std::forward<Args>(args)...);
    }

    /* private */

    template<typename ...Args>
    void Seeder::callUnfolded(const bool silent, Args &&...args)
    {
        callInternal(silent, [this, ...args = std::forward<Args>(args)]() mutable
        {
            run(std::forward<Args>(args)...);
        });
    }

    template<typename ...Args>
    void Seeder::callUnfolded(const bool silent, Args &&...args) const
    {
        callInternal(silent, [this, ...args = std::forward<Args>(args)]() mutable
        {
            run(std::forward<Args>(args)...);
        });
    }

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

// Import the most used constants into the Seeders namespace so the user doesn't have to
namespace Seeders
{
    /*! Alias for the DB facade. */
    using TINYORM_COMMON_NAMESPACE::Orm::DB; // NOLINT(misc-unused-using-decls)
    /*! Alias for the null QVariant-s utils. */
    using TINYORM_COMMON_NAMESPACE::Orm::Utils::NullVariant; // NOLINT(misc-unused-using-decls)
    /*! Alias for the Tom Seeder. */
    using TINYORM_COMMON_NAMESPACE::Tom::Seeder; // NOLINT(misc-unused-using-decls)

    // Aliases for the most used string constants
    /*! Alias for the string constant "id". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::ID; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "name". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::NAME; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "size". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::SIZE_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "created_at". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::CREATED_AT; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "updated_at". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::UPDATED_AT; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "deleted_at". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::DELETED_AT; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "null". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::null_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "note". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::NOTE; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "pivot". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::pivot_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "hash". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::HASH_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "progress". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::Progress; // NOLINT(misc-unused-using-decls)

    /*! Alias for the string constant "MySQL". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::MYSQL_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "PostgreSQL". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::POSTGRESQL; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "SQLite". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::SQLITE; // NOLINT(misc-unused-using-decls)

    /*! Alias for the string constant "driver". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::driver_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "host". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::host_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "port". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::port_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "database". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::database_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "schema". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::search_path; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "username". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::username_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "password". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::password_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "charset". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::charset_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "collation". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::collation_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "timezone". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::timezone_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "prefix". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::prefix_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "options". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::options_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "strict". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::strict_; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "engine". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::engine_; // NOLINT(misc-unused-using-decls)

    /*! Alias for the string constant "127.0.0.1". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::H127001; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "localhost". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::LOCALHOST; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "3306". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::P3306; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "5432". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::P5432; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "root". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::ROOT; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "UTC". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::UTC; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "LOCAL". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::LOCAL; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "DEFAULT". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::DEFAULT; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "SYSTEM". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::SYSTEM; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "+00:00". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::TZ00; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "public". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::PUBLIC; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "utf8". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::UTF8; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "utf8mb4". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::UTF8MB4; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "InnoDB". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::InnoDB; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "MyISAM". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::MyISAM; // NOLINT(misc-unused-using-decls)

    /*! Alias for the string constant "utf8_general_ci". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::UTF8Generalci; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "utf8_unicode_ci". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::UTF8Unicodeci; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "utf8mb4_unicode_520_ci". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::UTF8MB4Unicode520ci; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "utf8mb4_0900_ai_ci". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::UTF8MB40900aici; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "uca1400_nopad_ai_ci". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::Uca1400Nopadaici; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "ucs_basic". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::UcsBasic; // NOLINT(misc-unused-using-decls)
    /*! Alias for the string constant "POSIX". */
    using TINYORM_COMMON_NAMESPACE::Orm::Constants::POSIX_; // NOLINT(misc-unused-using-decls)

} // namespace Seeders

#endif // TOM_SEEDER_HPP
