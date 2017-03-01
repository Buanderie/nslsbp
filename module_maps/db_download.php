<?php

/* Creating database variable. */
require("mysql_credentials/config.inc.php");
$conn = new mysqli(DB_SERVER, DB_USER, DB_PASS, DB_DATABASE);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$sql = "SELECT * FROM ".TABLE_HOUSEKEEPING." WHERE `time_gps` > {$_GET['date_gps']} ORDER BY `time_gps` ASC";
$result = $conn->query($sql);
$gps = [];

if($result->num_rows > 0) {
  while($data = $result->fetch_assoc()) {
      $gps[] = [$data['row_id'], $data['time_local'],
                $data['time_gps'], $data['lat'], $data['lng'], $data['gspeed'],
                $data['sea_alt'], $data['geo_alt'],
                $data['cpu_temp'], $data['gpu_temp']];
  }
}

$sql = "SELECT * FROM ".TABLE_XBEE." WHERE `time_gps` > {$_GET['date_xbee']} ORDER BY `time_gps` ASC";
$result = $conn->query($sql);
$xbee = [];

if($result->num_rows > 0) {
  while($data = $result->fetch_assoc()) {
      $xbee[] = [$data['row_id'], $data['time_gps'],
                 $data['lat'], $data['lng'], $data['alt'],
                 $data['roll'], $data['pitch'], $data['yaw'],
                 $data['vsys'], $data['isys'], $data['wsys'],
                 $data['out_temp'], $data['gen_temp'],
                 $data['pay_temp'], $data['bat_temp'],
                 $data['tc_received'], $data['ping_received']];
  }
}

$json_data = array(
  "gps"  => $gps,
  "xbee" => $xbee
);

$conn->close();

/* Converting to JSON format */
$json = json_encode($json_data);
echo $json;

?>
