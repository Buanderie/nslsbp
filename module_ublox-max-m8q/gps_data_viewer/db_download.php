<?php
	
    // Creating database variable
    require("mysql-db-wrapper/config.inc.php");
    require("mysql-db-wrapper/Database.class.php");
    $db = new Database(DB_SERVER, DB_USER, DB_PASS, DB_DATABASE);

    // Connecting db
    $db->connect();

    // Get last GPS data
    $gps = array();
    $sql = "SELECT * FROM ".TABLE_GPS." WHERE `time` > {$_GET['date']} ORDER BY `time` ASC";
    $query = $db->query($sql);
    while($data = $db->fetch_array($query)) {
    	$gps[] = [$data['time'], $data['lat'], $data['lng']];
    }

	// Disconnecting db
    $db->close();
?>