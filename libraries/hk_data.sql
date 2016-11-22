-- phpMyAdmin SQL Dump
-- version 4.5.4.1deb2ubuntu2
-- http://www.phpmyadmin.net
--
-- Servidor: localhost
-- Temps de generació: 14-11-2016 a les 14:10:54
-- Versió del servidor: 5.7.16-0ubuntu0.16.04.1
-- Versió de PHP: 7.0.8-0ubuntu0.16.04.3

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Base de dades: `nslsbp`
--

-- --------------------------------------------------------

--
-- Estructura de la taula `hk_data`
--

CREATE TABLE `hk_data` (
  `row_id` int(10) NOT NULL,
  `time_local` int(11) NOT NULL,
  `time_gps` int(11) NOT NULL,
  `lat` float NOT NULL,
  `lng` float NOT NULL,
  `gspeed` float NOT NULL,
  `sea_alt` float NOT NULL,
  `geo_alt` float NOT NULL,
  `acc_x` float NOT NULL,
  `acc_y` float NOT NULL,
  `acc_z` float NOT NULL,
  `gyro_x` float NOT NULL,
  `gyro_y` float NOT NULL,
  `gyro_z` float NOT NULL,
  `mag_x` float NOT NULL,
  `mag_y` float NOT NULL,
  `mag_z` float NOT NULL,
  `cpu_temp` float NOT NULL,
  `gpu_temp` float NOT NULL,
  `in_temp` float NOT NULL,
  `in_pressure` float NOT NULL,
  `in_calc_alt` float NOT NULL,
  `out_temp` float NOT NULL,
  `out_pressure` float NOT NULL,
  `out_calc_alt` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Indexos per taules bolcades
--

--
-- Index de la taula `hk_data`
--
ALTER TABLE `hk_data`
  ADD PRIMARY KEY (`row_id`);

--
-- AUTO_INCREMENT per les taules bolcades
--

--
-- AUTO_INCREMENT per la taula `hk_data`
--
ALTER TABLE `hk_data`
  MODIFY `row_id` int(10) NOT NULL AUTO_INCREMENT;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
