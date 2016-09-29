/* Scripts used in map.html */
var cmplt_path = new Array();
var time_line  = new Array();
var gps_extra  = new Array();
// var cmplt_path = [
//             {lat: 37.772, lng: -122.214},
//             {lat: 21.291, lng: -157.821},
//             {lat: -18.142, lng: 178.431},
//             {lat: -27.467, lng: 153.027}
// ];
// var time_line = [1474022404, 1474022405, 1474022406, 1474022407];
var lineCoordinatesPath;
var map;
var map_marker;

/* On document ready actions: ------------------------------------------------------------------ */
$(document).ready(function() {
    //initMap();
    console.log("BBS map ready.");
});

function initMap() {
    map = new google.maps.Map(document.getElementById('map'), {
        center: {lat: 41.39, lng: 2.11},
        zoom: 6
    });
    lineCoordinatesPath = new google.maps.Polyline({
        path: cmplt_path,
        geodesic: true,
        strokeColor: '#2E10FF',
        strokeOpacity: 1.0,
        strokeWeight: 2
    });
    map_marker = new google.maps.Marker({position: {lat: 41.39, lng: 2.11}, map: map});

    map_marker.setMap(map);
    lineCoordinatesPath.setMap(map);

    var interval = window.setInterval(download_data, 5000);
}
function redraw() {
    // var waypoints = lineCoordinatesPath.getPath();
    // var last_waypoint = waypoints.getAt(waypoints.getLength() - 1);
    // var new_lat = last_waypoint.lat() + 0.01;
    // var new_lng = last_waypoint.lng() + 0.01;
    // waypoints.push(new google.maps.LatLng(new_lat, new_lng));
    // map.setCenter({lat: new_lat, lng: new_lng})
    // map_marker.setPosition({lat: new_lat, lng: new_lng});

    if(cmplt_path.length > 0) {
        lineCoordinatesPath.setPath(cmplt_path);
        map.setCenter({lat: cmplt_path[cmplt_path.length-1].lat, lng: cmplt_path[cmplt_path.length-1].lng});
        map_marker.setPosition({lat: cmplt_path[cmplt_path.length-1].lat, lng: cmplt_path[cmplt_path.length-1].lng});
        update_gps_tables();
    } else {
        console.log("NO DATA");
        map.setCenter({lat: 41.39, lng: 2.11});
    }

}

function download_data() {
    $.ajax({
        url: "db_download.php?date="+(time_line.length<1 ? 0 : time_line[time_line.length-1]),
        dataType: "text",
        success: function(data) {
            var new_data = $.parseJSON(data);
            new_data.forEach(function(gps) {
                cmplt_path.push({lat: parseFloat(gps[3]), lng: parseFloat(gps[4])});
                time_line.push(parseFloat(gps[2]));
                gps_extra.push({time_local: parseFloat(gps[1]), kph: parseFloat(gps[5]), sea: parseFloat(gps[6]),
                                geo: parseFloat(gps[7]), course: parseFloat(gps[8]),
                                temp: parseFloat(gps[9])});
            });
            if(new_data.length > 0){redraw();}
        }
            
    });
}