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
    $new_data = array();
    $old_data = array();
    $sql = "SELECT * FROM ".TABLE_GPS." ORDER BY `time` DESC LIMIT 1";
    $query = $db->query($sql);
    while($data = $db->fetch_array($query)) {
    	$old_data = [$data['time'], $data['lat'], $data['lng']];
    }

    $new_data = array(
		"time" => $old_data['time']+2,
		"lat" => $old_data['lat']+(2*mt_rand(-1,1)),
		"lng" => $old_data['lng']+(2*mt_rand(-1,1))
	);
    for($i=0;$i<MAX;$i++) {
    	$old_data = $new_data;
    	$new_data = array(
			"time" => $old_data['time']+2,
			"lat" => $old_data['lat']+(2*mt_rand(-1,1)),
			"lng" => $old_data['lng']+(2*mt_rand(-1,1))
		);
    }

	// Disconnecting db
    $db->close();
?>