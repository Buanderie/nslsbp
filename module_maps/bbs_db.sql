-- phpMyAdmin SQL Dump
-- version 4.0.10deb1
-- http://www.phpmyadmin.net
--
-- Servidor: localhost
-- Tiempo de generación: 29-09-2016 a las 11:46:29
-- Versión del servidor: 5.5.52-0ubuntu0.14.04.1
-- Versión de PHP: 5.5.9-1ubuntu4.19

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+01:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Base de datos: `bbs`
--

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `dbg`
--

CREATE TABLE IF NOT EXISTS `dbg` (
  `time` int(11) NOT NULL,
  `message` varchar(150) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `gps`
--

CREATE TABLE IF NOT EXISTS `gps` (
  `row_id` int(11) NOT NULL AUTO_INCREMENT,
  `time_local` int(11) NOT NULL,
  `time_gps` int(11) NOT NULL,
  `lat` varchar(30) NOT NULL,
  `lng` varchar(30) NOT NULL,
  `v_kph` varchar(30) NOT NULL,
  `sea_alt` varchar(30) NOT NULL,
  `geo_alt` varchar(30) NOT NULL,
  `course` varchar(30) NOT NULL,
  `temp` varchar(30) NOT NULL,
  PRIMARY KEY (`row_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=12 ;

--
-- Volcado de datos para la tabla `gps`
--

INSERT INTO `gps` (`row_id`, `time_local`, `time_gps`, `lat`, `lng`, `v_kph`, `sea_alt`, `geo_alt`, `course`, `temp`) VALUES
(1, 1475141350, 1475141346, '41.3', '2.2', '20', '100', '90', '1', '12'),
(2, 1475141351, 1475141348, '41.3', '4.2', '25', '100', '90', '1', '16'),
(3, 1475141353, 1475141350, '39.3', '2.2', '25', '100', '87', '1', '16'),
(4, 1475141355, 1475141352, '37.3', '0.2', '20', '105', '84', '1', '16'),
(5, 1475141357, 1475141354, '39.3', '0.2', '25', '100', '81', '1', '12'),
(6, 1475141359, 1475141356, '39.3', '2.2', '25', '105', '84', '1', '12'),
(7, 1475141361, 1475141358, '41.3', '4.2', '20', '110', '81', '1', '16'),
(8, 1475141363, 1475141360, '43.3', '6.2', '20', '110', '78', '1', '16'),
(9, 1475141365, 1475141362, '41.3', '6.2', '15', '115', '81', '1', '16'),
(10, 1475141367, 1475141364, '41.3', '6.2', '10', '120', '84', '1', '20'),
(11, 1475141369, 1475141366, '39.3', '8.2', '5', '120', '87', '1', '20');

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
