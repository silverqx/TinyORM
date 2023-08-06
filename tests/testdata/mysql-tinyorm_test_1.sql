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
-- Database: `tinyorm_test_1`
--
CREATE DATABASE IF NOT EXISTS `tinyorm_test_1` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci;
USE `tinyorm_test_1`;

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
  `note` varchar(255) DEFAULT NULL,
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- RELATIONSHIPS FOR TABLE `torrents`:
--

--
-- Dumping data for table `torrents`
--

INSERT INTO `torrents` (`id`, `name`, `size`, `progress`, `added_on`, `hash`, `note`, `created_at`, `updated_at`) VALUES
(1, 'test1', 11, 100, '2020-08-01 20:11:10', '1579e3af2768cdf52ec84c1f320333f68401dc6e', NULL, '2021-01-01 14:51:23', '2021-01-01 18:46:31'),
(2, 'test2', 12, 200, '2020-08-02 20:11:10', '2579e3af2768cdf52ec84c1f320333f68401dc6e', NULL, '2021-01-02 14:51:23', '2021-01-02 18:46:31'),
(3, 'test3', 13, 300, '2020-08-03 20:11:10', '3579e3af2768cdf52ec84c1f320333f68401dc6e', NULL, '2021-01-03 14:51:23', '2021-01-03 18:46:31'),
(4, 'test4', 14, 400, '2020-08-04 20:11:10', '4579e3af2768cdf52ec84c1f320333f68401dc6e', 'after update revert updated_at', '2021-01-04 14:51:23', '2021-01-04 18:46:31'),
(5, 'test5', 15, 500, '2020-08-05 20:11:10', '5579e3af2768cdf52ec84c1f320333f68401dc6e', 'no peers', '2021-01-05 14:51:23', '2021-01-05 18:46:31'),
(6, 'test6', 16, 600, '2020-08-06 20:11:10', '6579e3af2768cdf52ec84c1f320333f68401dc6e', 'no files no peers', '2021-01-06 14:51:23', '2021-01-06 18:46:31');

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
(7, 5, 1, 'test5_file2.mkv', 2560, 890, 'for tst_BaseModel::remove()/destroy()', '2021-01-02 14:55:23', '2021-01-02 17:47:31'),
(8, 5, 2, 'test5_file3.mkv', 2570, 896, 'for tst_BaseModel::destroy()', '2021-01-02 14:56:23', '2021-01-02 17:48:31');

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

-- --------------------------------------------------------

--
-- Table structure for table `torrent_tags`
--

DROP TABLE IF EXISTS `torrent_tags`;
CREATE TABLE `torrent_tags` (
  `id` bigint UNSIGNED NOT NULL,
  `name` varchar(255) NOT NULL,
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- RELATIONSHIPS FOR TABLE `torrent_tags`:
--

--
-- Dumping data for table `torrent_tags`
--

INSERT INTO `torrent_tags` (`id`, `name`, `created_at`, `updated_at`) VALUES
(1, 'tag1', '2021-01-11 11:51:28', '2021-01-11 23:47:11'),
(2, 'tag2', '2021-01-12 11:51:28', '2021-01-12 23:47:11'),
(3, 'tag3', '2021-01-13 11:51:28', '2021-01-13 23:47:11'),
(4, 'tag4', '2021-01-14 11:51:28', '2021-01-14 23:47:11');

-- --------------------------------------------------------

--
-- Table structure for table `tag_torrent`
--

DROP TABLE IF EXISTS `tag_torrent`;
CREATE TABLE `tag_torrent` (
  `torrent_id` bigint UNSIGNED NOT NULL,
  `tag_id` bigint UNSIGNED NOT NULL,
  `active` tinyint(1) NOT NULL DEFAULT '1',
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- RELATIONSHIPS FOR TABLE `tag_torrent`:
--   `torrent_id`
--       `torrents` -> `id`
--   `tag_id`
--       `torrent_tags` -> `id`
--

--
-- Dumping data for table `tag_torrent`
--

INSERT INTO `tag_torrent` (`torrent_id`, `tag_id`, `active`, `created_at`, `updated_at`) VALUES
(2, 1, 1, '2021-02-21 17:31:58', '2021-02-21 18:49:22'),
(2, 2, 1, '2021-02-22 17:31:58', '2021-02-22 18:49:22'),
(2, 3, 0, '2021-02-23 17:31:58', '2021-02-23 18:49:22'),
(2, 4, 1, '2021-02-24 17:31:58', '2021-02-24 18:49:22'),
(3, 2, 1, '2021-02-24 17:31:58', '2021-02-24 18:49:22'),
(3, 4, 1, '2021-02-24 17:31:58', '2021-02-24 18:49:22');

-- --------------------------------------------------------

--
-- Table structure for table `tag_properties`
--

DROP TABLE IF EXISTS `tag_properties`;
CREATE TABLE `tag_properties` (
  `id` bigint UNSIGNED NOT NULL,
  `tag_id` bigint UNSIGNED NOT NULL,
  `color` varchar(255) NOT NULL,
  `position` int UNSIGNED NOT NULL,
  `created_at` timestamp NULL DEFAULT NULL,
  `updated_at` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- RELATIONSHIPS FOR TABLE `tag_properties`:
--   `tag_id`
--       `torrent_tags` -> `id`
--

--
-- Dumping data for table `tag_properties`
--

INSERT INTO `tag_properties` (`id`, `tag_id`, `color`, `position`, `created_at`, `updated_at`) VALUES
(1, 1, 'white', 0, '2021-02-11 12:41:28', '2021-02-11 22:17:11'),
(2, 2, 'blue', 1, '2021-02-12 12:41:28', '2021-02-12 22:17:11'),
(3, 3, 'red', 2, '2021-02-13 12:41:28', '2021-02-13 22:17:11'),
(4, 4, 'orange', 3, '2021-02-14 12:41:28', '2021-02-14 22:17:11');

-- --------------------------------------------------------

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
CREATE TABLE `users` (
  `id` bigint UNSIGNED NOT NULL,
  `name` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- RELATIONSHIPS FOR TABLE `users`:
--

--
-- Dumping data for table `users`
--

INSERT INTO `users` (`id`, `name`) VALUES
(1, 'andrej'),
(2, 'silver'),
(3, 'peter');

-- --------------------------------------------------------

--
-- Table structure for table `roles`
--

DROP TABLE IF EXISTS `roles`;
CREATE TABLE `roles` (
  `id` bigint UNSIGNED NOT NULL,
  `name` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- RELATIONSHIPS FOR TABLE `roles`:
--

--
-- Dumping data for table `roles`
--

INSERT INTO `roles` (`id`, `name`) VALUES
(1, 'role one'),
(2, 'role two'),
(3, 'role three');

-- --------------------------------------------------------

--
-- Table structure for table `role_user`
--

DROP TABLE IF EXISTS `role_user`;
CREATE TABLE `role_user` (
  `role_id` bigint UNSIGNED NOT NULL,
  `user_id` bigint UNSIGNED NOT NULL,
  `active` tinyint(1) NOT NULL DEFAULT '1'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- RELATIONSHIPS FOR TABLE `role_user`:
--   `role_id`
--       `roles` -> `id`
--   `user_id`
--       `users` -> `id`
--

--
-- Dumping data for table `role_user`
--

INSERT INTO `role_user` (`role_id`, `user_id`, `active`) VALUES
(1, 1, 1),
(2, 1, 0),
(3, 1, 1),
(2, 2, 1);

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
  ADD KEY `torrents_name_unique` (`name`);

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
-- Indexes for table `torrent_tags`
--
ALTER TABLE `torrent_tags`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `torrent_tags_name_unique` (`name`);

--
-- Indexes for table `tag_torrent`
--
ALTER TABLE `tag_torrent`
  ADD PRIMARY KEY (`torrent_id`, `tag_id`),
  ADD KEY `tag_torrent_tag_id_foreign` (`tag_id`);

--
-- Indexes for table `tag_properties`
--
ALTER TABLE `tag_properties`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `tag_properties_position_unique` (`position`),
  ADD KEY `torrent_tags_tag_id_foreign` (`tag_id`);

--
-- Indexes for table `users`
--
ALTER TABLE `users`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `users_name_unique` (`name`);

--
-- Indexes for table `roles`
--
ALTER TABLE `roles`
ADD PRIMARY KEY (`id`),
ADD UNIQUE KEY `roles_name_unique` (`name`);

--
-- Indexes for table `role_user`
--
ALTER TABLE `role_user`
  ADD PRIMARY KEY (`role_id`, `user_id`),
  ADD KEY `role_user_user_id_foreign` (`user_id`);

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
-- AUTO_INCREMENT for table `torrent_tags`
--
ALTER TABLE `torrent_tags`
  MODIFY `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT for table `tag_properties`
--
ALTER TABLE `tag_properties`
  MODIFY `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT for table `users`
--
ALTER TABLE `users`
  MODIFY `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT for table `roles`
--
ALTER TABLE `roles`
  MODIFY `id` bigint UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

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

--
-- Constraints for table `tag_torrent`
--
ALTER TABLE `tag_torrent`
  ADD CONSTRAINT `tag_torrent_torrent_id_foreign` FOREIGN KEY (`torrent_id`) REFERENCES `torrents` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `tag_torrent_tag_id_foreign` FOREIGN KEY (`tag_id`) REFERENCES `torrent_tags` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `tag_properties`
--
ALTER TABLE `tag_properties`
  ADD CONSTRAINT `tag_properties_tag_id_foreign` FOREIGN KEY (`tag_id`) REFERENCES `torrent_tags` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `role_user`
--
ALTER TABLE `role_user`
  ADD CONSTRAINT `role_user_role_id_foreign` FOREIGN KEY (`role_id`) REFERENCES `roles` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `role_user_user_id_foreign` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

SET FOREIGN_KEY_CHECKS=1;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
