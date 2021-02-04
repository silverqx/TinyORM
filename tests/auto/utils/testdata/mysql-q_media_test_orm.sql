-- phpMyAdmin SQL Dump
-- version 5.1.0-rc1
-- https://www.phpmyadmin.net/
--
-- Host: merydeye-devel
-- Generation Time: Dec 26, 2020 at 10:19 AM
-- Server version: 8.0.22
-- PHP Version: 8.0.0

SET FOREIGN_KEY_CHECKS=0;
SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `q_tinyorm_test_1`
--
CREATE DATABASE IF NOT EXISTS `q_tinyorm_test_1` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci;
USE `q_tinyorm_test_1`;

-- --------------------------------------------------------

--
-- Table structure for table `settings`
--

DROP TABLE IF EXISTS `settings`;
CREATE TABLE `settings` (
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL DEFAULT '',
  `value` varchar(255) NOT NULL DEFAULT '',
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci COMMENT='No autoincrement and columns have default value';

--
-- RELATIONSHIPS FOR TABLE `settings`:
--

-- --------------------------------------------------------

--
-- Table structure for table `torrents`
--

DROP TABLE IF EXISTS `torrents`;
CREATE TABLE `torrents` (
  `id` bigint UNSIGNED NOT NULL,
  `name` varchar(255) NOT NULL COMMENT 'Torrent name',
  `size` bigint UNSIGNED NOT NULL DEFAULT '0',
  `progress` smallint UNSIGNED NOT NULL DEFAULT '0',
  `added_on` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `hash` varchar(40) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- RELATIONSHIPS FOR TABLE `torrents`:
--

--
-- Dumping data for table `torrents`
--

INSERT INTO `torrents` (`id`, `name`, `size`, `progress`, `added_on`, `hash`, `created_at`, `updated_at`) VALUES
(1, 'test1', 11, 100, '2020-08-01 20:11:10', '1579e3af2768cdf52ec84c1f320333f68401dc6e', '2021-01-01 14:51:23', '2021-01-01 17:46:31'),
(2, 'test2', 12, 200, '2020-08-02 20:11:10', '2579e3af2768cdf52ec84c1f320333f68401dc6e', '2021-01-02 14:51:23', '2021-01-02 17:46:31'),
(4, 'test4', 14, 400, '2020-08-04 20:11:10', '4579e3af2768cdf52ec84c1f320333f68401dc6e', '2021-01-03 14:51:23', '2021-01-03 17:46:31'),
(3, 'test3', 13, 300, '2020-08-03 20:11:10', '3579e3af2768cdf52ec84c1f320333f68401dc6e', '2021-01-04 14:51:23', '2021-01-04 17:46:31'),
(5, 'test5 no peers', 15, 500, '2020-08-05 20:11:10', '5579e3af2768cdf52ec84c1f320333f68401dc6e', '2021-01-05 14:51:23', '2021-01-05 17:46:31'),
(6, 'test6 no files no peers', 16, 600, '2020-08-06 20:11:10', '6579e3af2768cdf52ec84c1f320333f68401dc6e', '2021-01-06 14:51:23', '2021-01-06 17:46:31');

-- --------------------------------------------------------

--
-- Table structure for table `torrent_peers`
--

DROP TABLE IF EXISTS `torrent_peers`;
CREATE TABLE `torrent_peers` (
  `id` bigint UNSIGNED NOT NULL,
  `torrent_id` bigint UNSIGNED NOT NULL,
  `seeds` int DEFAULT NULL,
  `total_seeds` int DEFAULT NULL,
  `leechers` int NOT NULL,
  `total_leechers` int NOT NULL,
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- RELATIONSHIPS FOR TABLE `torrent_peers`:
--   `torrent_id`
--       `torrents` -> `id`
--

--
-- Dumping data for table `torrent_peers`
--

INSERT INTO `torrent_peers` (`id`, `torrent_id`, `seeds`, `total_seeds`, `leechers`, `total_leechers`, `created_at`, `updated_at`) VALUES
(1, 1, 1, 1, 1, 1, '2021-01-01 14:51:23', '2021-01-01 17:46:31'),
(2, 2, 2, 2, 2, 2, '2021-01-02 14:51:23', '2021-01-02 17:46:31'),
(3, 3, 3, 3, 3, 3, '2021-01-03 14:51:23', '2021-01-03 17:46:31'),
(4, 4, NULL, 4, 4, 4, '2021-01-04 14:51:23', '2021-01-04 17:46:31');

-- --------------------------------------------------------

--
-- Table structure for table `torrent_previewable_files`
--

DROP TABLE IF EXISTS `torrent_previewable_files`;
CREATE TABLE `torrent_previewable_files` (
  `id` bigint UNSIGNED NOT NULL,
  `torrent_id` bigint UNSIGNED NOT NULL,
  `file_index` int NOT NULL,
  `filepath` varchar(255) NOT NULL,
  `size` bigint UNSIGNED NOT NULL,
  `progress` smallint UNSIGNED NOT NULL,
  `note` varchar(255) DEFAULT NULL,
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- RELATIONSHIPS FOR TABLE `torrent_previewable_files`:
--   `torrent_id`
--       `torrents` -> `id`
--

--
-- Dumping data for table `torrent_previewable_files`
--

INSERT INTO `torrent_previewable_files` (`id`, `torrent_id`, `file_index`, `filepath`, `size`, `progress`, `note`, `created_at`, `updated_at`) VALUES
(1, 1, 0, 'test1_file1.mkv', 1024, 200, 'no file propreties', '2021-01-01 14:51:23', '2021-01-01 17:46:31'),
(2, 2, 0, 'test2_file1.mkv', 2048, 870, NULL, '2021-01-02 14:51:23', '2021-01-02 17:46:31'),
(3, 2, 1, 'test2_file2.mkv', 3072, 1000, NULL, '2021-01-02 14:51:23', '2021-01-02 17:46:31'),
(4, 3, 0, 'test3_file1.mkv', 5568, 870, NULL, '2021-01-03 14:51:23', '2021-01-03 17:46:31'),
(5, 4, 0, 'test4_file1.mkv', 4096, 0, NULL, '2021-01-04 14:51:23', '2021-01-04 17:46:31'),
(6, 5, 0, 'test5_file1.mkv', 2048, 999, NULL, '2021-01-05 14:51:23', '2021-01-05 17:46:31'),
(7, 2, 2, 'test2_file3.mkv', 2560, 890, 'for tst_BaseModel::remove()/destroy()', '2021-01-02 14:55:23', '2021-01-02 17:47:31'),
(8, 2, 3, 'test2_file4.mkv', 2570, 896, 'for tst_BaseModel::destroy()', '2021-01-02 14:56:23', '2021-01-02 17:48:31');

-- --------------------------------------------------------

--
-- Table structure for table `torrent_previewable_file_properties`
--

DROP TABLE IF EXISTS `torrent_previewable_file_properties`;
CREATE TABLE `torrent_previewable_file_properties` (
  `id` bigint UNSIGNED NOT NULL,
  `previewable_file_id` bigint UNSIGNED NOT NULL,
  `name` varchar(255) NOT NULL,
  `size` bigint UNSIGNED NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- RELATIONSHIPS FOR TABLE `torrent_previewable_file_properties`:
--   `previewable_file_id`
--       `torrent_previewable_files` -> `id`
--

--
-- Dumping data for table `torrent_previewable_file_properties`
--

INSERT INTO `torrent_previewable_file_properties` (`id`, `previewable_file_id`, `name`, `size`) VALUES
(1, 2, 'test2_file1', 2),
(2, 3, 'test2_file2', 2),
(3, 4, 'test3_file1', 4),
(4, 5, 'test4_file1', 5),
(5, 6, 'test5_file1', 6);

-- --------------------------------------------------------

--
-- Table structure for table `file_property_properties`
--

DROP TABLE IF EXISTS `file_property_properties`;
CREATE TABLE `file_property_properties` (
`id` bigint UNSIGNED NOT NULL,
`file_property_id` bigint UNSIGNED NOT NULL,
`name` varchar(255) NOT NULL,
`value` bigint UNSIGNED NOT NULL,
`created_at` timestamp NULL DEFAULT NULL,
`updated_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- RELATIONSHIPS FOR TABLE `file_property_properties`:
--   `file_property_id`
--       `torrent_previewable_file_properties` -> `id`
--

--
-- Dumping data for table `file_property_properties`
--

INSERT INTO `file_property_properties` (`id`, `file_property_id`, `name`, `value`, `created_at`, `updated_at`) VALUES
(1, 1, 'test2_file1_property1', 1, '2021-01-01 14:51:23', '2021-01-01 17:46:31'),
(2, 2, 'test2_file2_property1', 2, '2021-01-02 14:51:23', '2021-01-02 17:46:31'),
(3, 3, 'test3_file1_property1', 3, '2021-01-03 14:51:23', '2021-01-03 17:46:31'),
(4, 3, 'test3_file1_property2', 4, '2021-01-04 14:51:23', '2021-01-04 17:46:31'),
(5, 4, 'test4_file1_property1', 5, '2021-01-05 14:51:23', '2021-01-05 17:46:31'),
(6, 5, 'test5_file1_property1', 6, '2021-01-06 14:51:23', '2021-01-06 17:46:31');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `settings`
--
ALTER TABLE `settings`
  ADD UNIQUE KEY `settings_name_unique` (`name`);

--
-- Indexes for table `torrents`
--
ALTER TABLE `torrents`
  ADD PRIMARY KEY (`id`),
  ADD KEY `torrents_name_index` (`name`);

--
-- Indexes for table `torrent_peers`
--
ALTER TABLE `torrent_peers`
  ADD PRIMARY KEY (`id`),
  ADD KEY `torrent_peers_torrent_id_foreign` (`torrent_id`);

--
-- Indexes for table `torrent_previewable_files`
--
ALTER TABLE `torrent_previewable_files`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `torrent_previewable_files_filepath_unique` (`filepath`),
  ADD KEY `torrent_previewable_files_torrent_id_foreign` (`torrent_id`);

--
-- Indexes for table `torrent_previewable_file_properties`
--
ALTER TABLE `torrent_previewable_file_properties`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `torrent_previewable_file_properties_name_unique` (`name`) USING BTREE,
  ADD KEY `torrent_previewable_file_properties_previewable_file_id_foreign` (`previewable_file_id`);

--
-- Indexes for table `file_property_properties`
--
ALTER TABLE `file_property_properties`
ADD PRIMARY KEY (`id`),
ADD UNIQUE KEY `file_property_properties_name_unique` (`name`) USING BTREE,
ADD KEY `file_property_properties_file_property_id_foreign` (`file_property_id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `torrents`
--
ALTER TABLE `torrents`
  MODIFY `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=7;

--
-- AUTO_INCREMENT for table `torrent_peers`
--
ALTER TABLE `torrent_peers`
  MODIFY `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT for table `torrent_previewable_files`
--
ALTER TABLE `torrent_previewable_files`
  MODIFY `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=7;

--
-- AUTO_INCREMENT for table `torrent_previewable_file_properties`
--
ALTER TABLE `torrent_previewable_file_properties`
  MODIFY `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;

--
-- AUTO_INCREMENT for table `file_property_properties`
--
ALTER TABLE `file_property_properties`
MODIFY `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=7;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `torrent_peers`
--
ALTER TABLE `torrent_peers`
  ADD CONSTRAINT `torrent_peers_torrent_id_foreign` FOREIGN KEY (`torrent_id`) REFERENCES `torrents` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `torrent_previewable_files`
--
ALTER TABLE `torrent_previewable_files`
  ADD CONSTRAINT `torrent_previewable_files_torrent_id_foreign` FOREIGN KEY (`torrent_id`) REFERENCES `torrents` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `torrent_previewable_file_properties`
--
ALTER TABLE `torrent_previewable_file_properties`
  ADD CONSTRAINT `torrent_previewable_file_properties_previewable_file_id_foreign` FOREIGN KEY (`previewable_file_id`) REFERENCES `torrent_previewable_files` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `file_property_properties`
--
ALTER TABLE `file_property_properties`
ADD CONSTRAINT `file_property_properties_file_property_id_foreign` FOREIGN KEY (`file_property_id`) REFERENCES `torrent_previewable_file_properties` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

SET FOREIGN_KEY_CHECKS=1;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
