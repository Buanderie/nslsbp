<?php
	
    // Creating database variable
    require("mysql-db-wrapper/config.inc.php");
    require("mysql-db-wrapper/Database.class.php");
    $db = new Database(DB_SERVER, DB_USER, DB_PASS, DB_DATABASE);

    // Connecting db
    $db->connect();

    // Get last GPS data
    $gps = array();
    $sql = "SELECT * FROM ".TABLE_GPS." WHERE `time_gps` > {$_GET['date']} ORDER BY `time_gps` ASC";
    echo($sql);
    $query = $db->query($sql);
    while($data = $db->fetch_array($query)) {
    	$gps[] = [$data['id'], $data['time_local'], $data['time_gps'] $data['lat'], $data['lng'], $data['v_kph'],
                 $data['sea_alt'], $data['geo_alt'], $data['course'], $data['temp']];
    }

	// Disconnecting db
    $db->close();
    
    // Converting to JSON format
    $json = json_encode($gps);
    echo $json;
?>