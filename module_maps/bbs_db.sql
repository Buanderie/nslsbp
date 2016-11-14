-- phpMyAdmin SQL Dump
-- version 4.4.13.1deb1
-- http://www.phpmyadmin.net
--
-- Servidor: localhost
-- Temps de generació: 01-10-2016 a les 19:38:32
-- Versió del servidor: 5.6.31-0ubuntu0.15.10.1
-- Versió de PHP: 5.6.11-1ubuntu3.4

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Base de dades: `barbie_space_explorer`
--

-- --------------------------------------------------------

--
-- Estructura de la taula `gps`
--

CREATE TABLE IF NOT EXISTS `gps` (
  `row_id` int(11) NOT NULL,
  `time_local` int(11) NOT NULL,
  `time_gps` int(11) NOT NULL,
  `lat` varchar(30) NOT NULL,
  `lng` varchar(30) NOT NULL,
  `v_kph` varchar(30) NOT NULL,
  `sea_alt` varchar(30) NOT NULL,
  `geo_alt` varchar(30) NOT NULL,
  `course` varchar(30) NOT NULL,
  `temp` varchar(30) NOT NULL,
  `cpu_temp` varchar(10) NOT NULL,
  `gpu_temp` varchar(10) NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=77 DEFAULT CHARSET=latin1;

--
-- Bolcant dades de la taula `gps`
--

--
-- Indexos per taules bolcades
--

--
-- Index de la taula `gps`
--
ALTER TABLE `gps`
  ADD PRIMARY KEY (`row_id`);

--
-- AUTO_INCREMENT per les taules bolcades
--

--
-- AUTO_INCREMENT per la taula `gps`
--
ALTER TABLE `gps`
  MODIFY `row_id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=1;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
