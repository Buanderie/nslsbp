<?php

/* Creating database variable. */
require("mysql_credentials/config.inc.php");
$conn = new mysqli(DB_SERVER, DB_USER, DB_PASS, DB_DATABASE);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$sql = "SELECT * FROM ".TABLE_HOUSEKEEPING." WHERE `time_gps` > {$_GET['date']} ORDER BY `time_gps` ASC";
$result = $conn->query($sql);
$gps = [];

if($result->num_rows > 0) {
  while($data = $result->fetch_assoc()) {
      $gps[] = [ $data['row_id'], $data['time_local'],
                 $data['time_gps'], $data['lat'], $data['lng'], $data['gspeed'],
                 $data['sea_alt'], $data['geo_alt'],
                 $data['acc_x'], $data['acc_y'], $data['acc_z'],
                 $data['gyro_x'], $data['gyro_y'], $data['gyro_z'],
                 $data['mag_x'], $data['mag_y'], $data['mag_z'],
                 $data['cpu_temp'], $data['gpu_temp'],
                 $data['in_temp'], $data['in_pressure'],
                 $data['out_temp'], $data['out_pressure'] ];
  }
}

$conn->close();

/* Converting to JSON format */
$json = json_encode($gps);
echo $json;

?>
