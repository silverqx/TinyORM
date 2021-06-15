<?php

require_once 'vendor/autoload.php';

use Illuminate\Database\Capsule\Manager as Capsule;
use Illuminate\Database\Schema\Blueprint;

/**
 * Combine Insert statement values with columns.
 *
 * @param array $columns
 * @param array $values
 *
 * @return array
 */
function combineValues(array $columns, array $values): array
{
    $result = [];
    $columnsSize = count($columns);

    foreach ($values as $value) {
        if (count($value) != $columnsSize)
            throw new InvalidArgumentException(
                '\'$columns\' and \'$values\' in the parseInsertValues() have to have ' .
                'the same elements size.');

        $result[] = array_combine($columns, $value);
    }

    return $result;
}

/**
 * Drop all tables for the given connection.
 *
 * @param string $connection The connection name
 * @return void
 */
function dropAllTables(string $connection)
{
    Capsule::schema($connection)->dropAllTables();
}

/**
 * Add all configuration connections to the capsule.
 *
 * @param Capsule $capsule
 * @param array   $configs
 */
function addConnections(Capsule $capsule, array $configs = [])
{
    foreach ($configs as $name => $config)
        $capsule->addConnection($config, $name);
}

/**
 * Create all tables for the given connection.
 *
 * @param string $connection The connection name
 * @return void
 */
function createTables(string $connection)
{
    $schema = Capsule::schema($connection);

    $schema->create('settings', function (Blueprint $table) {
        $table->string('name')->default('')->unique();
        $table->string('value')->default('');
        $table->timestamps();
    });

    $schema->create('torrents', function (Blueprint $table) {
        $table->id();
        $table->string('name')->unique()->comment('Torrent name');
        $table->unsignedBigInteger('size')->default('0');
        $table->unsignedSmallInteger('progress')->default('0');
        $table->dateTime('added_on')->useCurrent();
        $table->string('hash', 40);
        $table->string('note')->nullable();
        $table->timestamps();
    });

    $schema->create('torrent_peers', function (Blueprint $table) {
        $table->id();
        $table->unsignedBigInteger('torrent_id');
        $table->integer('seeds')->nullable();
        $table->integer('total_seeds')->nullable();
        $table->integer('leechers');
        $table->integer('total_leechers');
        $table->timestamps();

        $table->foreign('torrent_id')->references('id')->on('torrents')
            ->cascadeOnUpdate()->cascadeOnDelete();
    });

    $schema->create('torrent_previewable_files', function (Blueprint $table) {
        $table->id();
        $table->unsignedBigInteger('torrent_id')->nullable();
        $table->integer('file_index');
        $table->string('filepath')->unique();
        $table->unsignedBigInteger('size');
        $table->unsignedSmallInteger('progress');
        $table->string('note')->nullable();
        $table->timestamps();

        $table->foreign('torrent_id')->references('id')->on('torrents')
            ->cascadeOnUpdate()->cascadeOnDelete();
    });

    $schema->create('torrent_previewable_file_properties', function (Blueprint $table) {
        $table->id();
        $table->unsignedBigInteger('previewable_file_id');
        $table->string('name')->unique();
        $table->unsignedBigInteger('size');
        $table->timestamps();

        $table->foreign('previewable_file_id')->references('id')
            ->on('torrent_previewable_files')
            ->cascadeOnUpdate()->cascadeOnDelete();
    });

    $schema->create('file_property_properties', function (Blueprint $table) {
        $table->id();
        $table->unsignedBigInteger('file_property_id');
        $table->string('name')->unique();
        $table->unsignedBigInteger('value');
        $table->timestamps();

        $table->foreign('file_property_id')->references('id')
            ->on('torrent_previewable_file_properties')
            ->cascadeOnUpdate()->cascadeOnDelete();
    });

    $schema->create('torrent_tags', function (Blueprint $table) {
        $table->id();
        $table->string('name')->unique();
        $table->string('note')->nullable();
        $table->timestamps();
    });

    $schema->create('tag_torrent', function (Blueprint $table) {
        $table->unsignedBigInteger('torrent_id');
        $table->unsignedBigInteger('tag_id');
        $table->boolean('active')->default(1);
        $table->timestamps();

        $table->primary(['torrent_id', 'tag_id']);

        $table->foreign('torrent_id')->references('id')->on('torrents')
            ->cascadeOnUpdate()->cascadeOnDelete();
        $table->foreign('tag_id')->references('id')->on('torrent_tags')
            ->cascadeOnUpdate()->cascadeOnDelete();
    });

    $schema->create('tag_properties', function (Blueprint $table) {
        $table->id();
        $table->unsignedBigInteger('tag_id');
        $table->string('color');
        $table->unsignedInteger('position')->unique();
        $table->timestamps();

        $table->foreign('tag_id')->references('id')->on('torrent_tags')
            ->cascadeOnUpdate()->cascadeOnDelete();
    });

    $schema->create('users', function (Blueprint $table) {
        $table->id();
        $table->string('name')->unique();
    });

    $schema->create('roles', function (Blueprint $table) {
        $table->id();
        $table->string('name')->unique();
    });

    $schema->create('role_user', function (Blueprint $table) {
        $table->unsignedBigInteger('role_id');
        $table->unsignedBigInteger('user_id');
        $table->boolean('active')->default(1);

        $table->primary(['role_id', 'user_id']);

        $table->foreign('role_id')->references('id')->on('roles')
            ->cascadeOnUpdate()->cascadeOnDelete();
        $table->foreign('user_id')->references('id')->on('users')
            ->cascadeOnUpdate()->cascadeOnDelete();
    });

    $schema->create('user_phones', function (Blueprint $table) {
        $table->id();
        $table->unsignedBigInteger('user_id');
        $table->string('number')->unique();

        $table->foreign('user_id')->references('id')->on('users')
            ->cascadeOnUpdate()->cascadeOnDelete();
    });
}

/**
 * Seed all tables with data.
 *
 * @param string $connection The connection name
 * @return void
 */
function seedTables(string $connection)
{
    Capsule::table('torrents', null, $connection)->insert(
        combineValues(['id', 'name', 'size', 'progress', 'added_on', 'hash', 'note', 'created_at', 'updated_at'], [
            [1, 'test1', 11, 100, '2020-08-01 20:11:10', '1579e3af2768cdf52ec84c1f320333f68401dc6e', NULL, '2021-01-01 14:51:23', '2021-01-01 18:46:31'],
            [2, 'test2', 12, 200, '2020-08-02 20:11:10', '2579e3af2768cdf52ec84c1f320333f68401dc6e', NULL, '2021-01-02 14:51:23', '2021-01-02 18:46:31'],
            [3, 'test3', 13, 300, '2020-08-03 20:11:10', '3579e3af2768cdf52ec84c1f320333f68401dc6e', NULL, '2021-01-03 14:51:23', '2021-01-03 18:46:31'],
            [4, 'test4', 14, 400, '2020-08-04 20:11:10', '4579e3af2768cdf52ec84c1f320333f68401dc6e', 'after update revert updated_at', '2021-01-04 14:51:23', '2021-01-04 18:46:31'],
            [5, 'test5', 15, 500, '2020-08-05 20:11:10', '5579e3af2768cdf52ec84c1f320333f68401dc6e', 'no peers',                       '2021-01-05 14:51:23', '2021-01-05 18:46:31'],
            [6, 'test6', 16, 600, '2020-08-06 20:11:10', '6579e3af2768cdf52ec84c1f320333f68401dc6e', 'no files no peers',              '2021-01-06 14:51:23', '2021-01-06 18:46:31'],
        ]));

    Capsule::table('torrent_peers', null, $connection)->insert(
        combineValues(['id', 'torrent_id', 'seeds', 'total_seeds', 'leechers', 'total_leechers', 'created_at', 'updated_at'], [
            [1, 1, 1,    1, 1, 1, '2021-01-01 14:51:23', '2021-01-01 17:46:31'],
            [2, 2, 2,    2, 2, 2, '2021-01-02 14:51:23', '2021-01-02 17:46:31'],
            [3, 3, 3,    3, 3, 3, '2021-01-03 14:51:23', '2021-01-03 17:46:31'],
            [4, 4, NULL, 4, 4, 4, '2021-01-04 14:51:23', '2021-01-04 17:46:31'],
        ]));

    Capsule::table('torrent_previewable_files', null, $connection)->insert(
        combineValues(['id', 'torrent_id', 'file_index', 'filepath', 'size', 'progress', 'note', 'created_at', 'updated_at'], [
            [1, 1,    0, 'test1_file1.mkv', 1024, 200,  'no file properties', '2021-01-01 14:51:23', '2021-01-01 17:46:31'],
            [2, 2,    0, 'test2_file1.mkv', 2048, 870,  NULL, '2021-01-02 14:51:23', '2021-01-02 17:46:31'],
            [3, 2,    1, 'test2_file2.mkv', 3072, 1000, NULL, '2021-01-02 14:51:23', '2021-01-02 17:46:31'],
            [4, 3,    0, 'test3_file1.mkv', 5568, 870,  NULL, '2021-01-03 14:51:23', '2021-01-03 17:46:31'],
            [5, 4,    0, 'test4_file1.mkv', 4096, 0,    NULL, '2021-01-04 14:51:23', '2021-01-04 17:46:31'],
            [6, 5,    0, 'test5_file1.mkv', 2048, 999,  NULL, '2021-01-05 14:51:23', '2021-01-05 17:46:31'],
            [7, 5,    1, 'test5_file2.mkv', 2560, 890,  'for tst_BaseModel::remove()/destroy()', '2021-01-02 14:55:23', '2021-01-02 17:47:31'],
            [8, 5,    2, 'test5_file3.mkv', 2570, 896,  'for tst_BaseModel::destroy()',          '2021-01-02 14:56:23', '2021-01-02 17:48:31'],
            [9, NULL, 0, 'test0_file0.mkv', 1440, 420,  'no torrent parent model',               '2021-01-02 14:56:23', '2021-01-02 17:48:31'],
        ]));

    Capsule::table('torrent_previewable_file_properties', null, $connection)->insert(
        combineValues(['id', 'previewable_file_id', 'name', 'size'], [
            [1, 2, 'test2_file1', 2],
            [2, 3, 'test2_file2', 2],
            [3, 4, 'test3_file1', 4],
            [4, 5, 'test4_file1', 5],
            [5, 6, 'test5_file1', 6],
        ]));

    Capsule::table('file_property_properties', null, $connection)->insert(
        combineValues(['id', 'file_property_id', 'name', 'value', 'created_at', 'updated_at'], [
            [1, 1, 'test2_file1_property1', 1, '2021-01-01 14:51:23', '2021-01-01 17:46:31'],
            [2, 2, 'test2_file2_property1', 2, '2021-01-02 14:51:23', '2021-01-02 17:46:31'],
            [3, 3, 'test3_file1_property1', 3, '2021-01-03 14:51:23', '2021-01-03 17:46:31'],
            [4, 3, 'test3_file1_property2', 4, '2021-01-04 14:51:23', '2021-01-04 17:46:31'],
            [5, 4, 'test4_file1_property1', 5, '2021-01-05 14:51:23', '2021-01-05 17:46:31'],
            [6, 5, 'test5_file1_property1', 6, '2021-01-06 14:51:23', '2021-01-06 17:46:31'],
        ]));

    Capsule::table('torrent_tags', null, $connection)->insert(
        combineValues(['id', 'name', 'note', 'created_at', 'updated_at'], [
            [1, 'tag1', null, '2021-01-11 11:51:28', '2021-01-11 23:47:11'],
            [2, 'tag2', null, '2021-01-12 11:51:28', '2021-01-12 23:47:11'],
            [3, 'tag3', null, '2021-01-13 11:51:28', '2021-01-13 23:47:11'],
            [4, 'tag4', null, '2021-01-14 11:51:28', '2021-01-14 23:47:11'],
            [5, 'tag5', null, '2021-01-15 11:51:28', '2021-01-15 23:47:11'],
        ]));

    Capsule::table('tag_torrent', null, $connection)->insert(
        combineValues(['torrent_id', 'tag_id', 'active', 'created_at', 'updated_at'], [
            [2, 1, 1, '2021-02-21 17:31:58', '2021-02-21 18:49:22'],
            [2, 2, 1, '2021-02-22 17:31:58', '2021-02-22 18:49:22'],
            [2, 3, 0, '2021-02-23 17:31:58', '2021-02-23 18:49:22'],
            [2, 4, 1, '2021-02-24 17:31:58', '2021-02-24 18:49:22'],
            [3, 2, 1, '2021-02-25 17:31:58', '2021-02-25 18:49:22'],
            [3, 4, 1, '2021-02-26 17:31:58', '2021-02-26 18:49:22'],
        ]));

    Capsule::table('tag_properties', null, $connection)->insert(
        combineValues(['id', 'tag_id', 'color', 'position', 'created_at', 'updated_at'], [
            [1, 1, 'white',  0, '2021-02-11 12:41:28', '2021-02-11 22:17:11'],
            [2, 2, 'blue',   1, '2021-02-12 12:41:28', '2021-02-12 22:17:11'],
            [3, 3, 'red',    2, '2021-02-13 12:41:28', '2021-02-13 22:17:11'],
            [4, 4, 'orange', 3, '2021-02-14 12:41:28', '2021-02-14 22:17:11'],
        ]));

    Capsule::table('users', null, $connection)->insert(
        combineValues(['id', 'name'], [
            [1, 'andrej'],
            [2, 'silver'],
            [3, 'peter'],
        ]));

    Capsule::table('roles', null, $connection)->insert(
        combineValues(['id', 'name'], [
            [1, 'role one'],
            [2, 'role two'],
            [3, 'role three'],
        ]));

    Capsule::table('role_user', null, $connection)->insert(
        combineValues(['role_id', 'user_id', 'active'], [
            [1, 1, 1],
            [2, 1, 0],
            [3, 1, 1],
            [2, 2, 1],
        ]));

    Capsule::table('user_phones', null, $connection)->insert(
        combineValues(['id', 'user_id', 'number'], [
            [1, 1, '914111000'],
            [2, 2, '902555777'],
            [3, 3, '905111999'],
        ]));
}

/**
 * Fix sequence number for Postgres.
 *
 * I have to fix sequences in Postgres because I'm inserting IDs manually and
 * it doesn't increment sequences.
 *
 * @return void
 */
function fixSequences() {
    $sequences = [
        'torrents_id_seq'                            => 7,
        'torrent_peers_id_seq'                       => 5,
        'torrent_previewable_files_id_seq'           => 10,
        'torrent_previewable_file_properties_id_seq' => 6,
        'file_property_properties_id_seq'            => 7,
        'torrent_tags_id_seq'                        => 6,
        'tag_properties_id_seq'                      => 5,
        'users_id_seq'                               => 4,
        'roles_id_seq'                               => 4,
        'user_phones_id_seq'                         => 4,
    ];

    foreach ($sequences as $sequence => $id) {
        Capsule::connection('pgsql')
            ->unprepared("ALTER SEQUENCE {$sequence} RESTART WITH {$id}");
    }
}

/**
 * Create and seed all tables for all connections.
 *
 * @param array $connections Connection names
 * @return void
 */
function createAndSeedTables(array $connections)
{
    foreach ($connections as $connection) {
        dropAllTables($connection);
        createTables($connection);
        seedTables($connection);

        if ($connection === 'pgsql')
            fixSequences();
    }
}

$capsule = new Capsule;

$capsule->setAsGlobal();
$capsule->bootEloquent();

$configs = [
    'mysql' => [
        'driver'    => 'mysql',
        'host'      => \getenv('DB_MYSQL_HOST')      ?: '127.0.0.1',
        'port'      => \getenv('DB_MYSQL_PORT')      ?: '3306',
        'database'  => \getenv('DB_MYSQL_DATABASE')  ?: '',
        'username'  => \getenv('DB_MYSQL_USERNAME')  ?: 'root',
        'password'  => \getenv('DB_MYSQL_PASSWORD')  ?: '',
        'charset'   => \getenv('DB_MYSQL_CHARSET')   ?: 'utf8mb4',
        'collation' => \getenv('DB_MYSQL_COLLATION') ?: 'utf8mb4_0900_ai_ci',
        'timezone'  => '+00:00',
        'prefix'    => '',
    ],

    'sqlite' => [
        'driver'   => 'sqlite',
        'database' => \getenv('DB_SQLITE_DATABASE') ?: '',
        'prefix'   => '',
        'foreign_key_constraints' => true,
    ],

    'pgsql' => [
        'driver'   => 'pgsql',
        'host'     => \getenv('DB_PGSQL_HOST')     ?: '127.0.0.1',
        'port'     => \getenv('DB_PGSQL_PORT')     ?: '5432',
        'database' => \getenv('DB_PGSQL_DATABASE') ?: 'postgres',
        'schema'   => \getenv('DB_PGSQL_SCHEMA')   ?: 'public',
        'username' => \getenv('DB_PGSQL_USERNAME') ?: 'postgres',
        'password' => \getenv('DB_PGSQL_PASSWORD') ?: '',
        'charset'  => \getenv('DB_PGSQL_CHARSET')  ?: 'utf8',
        'timezone' => 'UTC',
        'sslmode'  => 'prefer',
        'prefix'   => '',
    ],
];

addConnections($capsule, $configs);
createAndSeedTables(array_keys($configs));
