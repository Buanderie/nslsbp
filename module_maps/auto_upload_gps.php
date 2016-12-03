<?php
	// Constants definition
	define(MAX, 10);

	// Creating database variable
    require("mysql-db-wrapper/config.inc.php");
    require("mysql-db-wrapper/Database.class.php");
    $db = new Database(DB_SERVER, DB_USER, DB_PASS, DB_DATABASE);

    // Connecting db
    $db->connect();

    // Create new random GPS data
    $sql = "SELECT * FROM ".TABLE_GPS." ORDER BY `row_id` DESC LIMIT 1";
    $query = $db->query($sql);
    while($data = $db->fetch_array($query)) {
    	$old_data = array(
            "time_local" => $data['time_local'],
            "time_gps" => $data['time_gps'],
            "lat" => $data['lat'],
            "lng" => $data['lng'],
            "v_kph" => $data['v_kph'],
            "sea_alt" => $data['sea_alt'],
            "geo_alt" => $data['geo_alt'],
            "course" => $data['course'],
            "temp" => $data['temp']
        );
    }

    for($i=0;$i<MAX;$i++) {
    	$new_data = array(
            "time_local" => $old_data['time_gps']+5,
            "time_gps" => $old_data['time_gps']+2,
            "lat" => $old_data['lat']+(2*mt_rand(-1,1)),
            "lng" => $old_data['lng']+(2*mt_rand(-1,1)),
            "v_kph" => $old_data['v_kph']+(5*mt_rand(-1,1)),
            "sea_alt" => $old_data['sea_alt']+(5*mt_rand(-1,1)),
            "geo_alt" => $old_data['geo_alt']+(3*mt_rand(-1,1)),
            "course" => $old_data['course'],
            "temp" => $old_data['temp']+(4*mt_rand(-1,1))
        );
        $db->query_insert(TABLE_GPS,$new_data);
        $old_data = $new_data;
    }

    echo MAX." rows inserted!";

	// Disconnecting db
    $db->close();
?>