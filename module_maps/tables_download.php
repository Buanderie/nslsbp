<?php

/* Creating database variable. */
require("mysql_credentials/config.inc.php");
$conn = new mysqli(DB_SERVER, DB_USER, DB_PASS, DB_DATABASE);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$sql = "SELECT * FROM ".TABLE_HOUSEKEEPING." ORDER BY `row_id` DESC LIMIT 1";
$result = $conn->query($sql);
$gps = [];

if($result->num_rows > 0) {
  while($data = $result->fetch_assoc()) {
      $gps = array(
        "row_id" => $data['row_id'],
        "time_local" => $data['time_local'],
        "time_gps" => $data['time_gps'],
        "lat" => $data['lat'],
        "lng" => $data['lng'],
        "gspeed" => $data['gspeed'],
        "sea_alt" => $data['sea_alt'],
        "geo_alt" => $data['geo_alt'],
        "cpu_temp" => $data['cpu_temp'],
        "gpu_temp" => $data['gpu_temp']
      );
  }
}

$sql = "SELECT * FROM ".TABLE_XBEE." ORDER BY `row_id` DESC LIMIT 1";
$result = $conn->query($sql);
$xbee = [];

if($result->num_rows > 0) {
  while($data = $result->fetch_assoc()) {
      $xbee = array(
              "row_id" => $data['row_id'],
              "time_gps" => $data['time_gps'],
              "lat" => $data['lat'],
              "lng" => $data['lng'],
              "alt" => $data['alt'],
              "roll" => $data['roll'],
              "pitch" => $data['pitch'],
              "yaw" => $data['yaw'],
              "vsys" => $data['vsys'],
              "isys" => $data['isys'],
              "wsys" => $data['wsys'],
              "out_temp" => $data['out_temp'],
              "gen_temp" => $data['gen_temp'],
              "pay_temp" => $data['pay_temp'],
              "bat_temp" => $data['bat_temp'],
              "tc_received" => $data['tc_received'],
              "ping_received" => $data['ping_received']
      );
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
