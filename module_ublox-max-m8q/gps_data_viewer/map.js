/* Scripts used in map.html */
var last_date = 0;
// var cmplt_path = new Array();
var cmplt_path = [
            {lat: 37.772, lng: -122.214},
            {lat: 21.291, lng: -157.821},
            {lat: -18.142, lng: 178.431},
            {lat: -27.467, lng: 153.027}
];
var time_line = [1474022404, 1474022405, 1474022406, 1474022407];
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
        center: {lat: -27.467, lng: 153.027},
        zoom: 8
    });
    lineCoordinatesPath = new google.maps.Polyline({
        path: cmplt_path,
        geodesic: true,
        strokeColor: '#2E10FF',
        strokeOpacity: 1.0,
        strokeWeight: 2
    });
    map_marker = new google.maps.Marker({position: {lat: -27.467, lng: 153.027}, map: map});

    map_marker.setMap(map);
    lineCoordinatesPath.setMap(map);

    var interval = window.setInterval(redraw, 5000);
}
function redraw() {
    // var waypoints = lineCoordinatesPath.getPath();
    // var last_waypoint = waypoints.getAt(waypoints.getLength() - 1);
    // var new_lat = last_waypoint.lat() + 0.01;
    // var new_lng = last_waypoint.lng() + 0.01;
    // waypoints.push(new google.maps.LatLng(new_lat, new_lng));
    // map.setCenter({lat: new_lat, lng: new_lng})
    // map_marker.setPosition({lat: new_lat, lng: new_lng});


    // var last_waypoint = cmplt_path[cmplt_path.length - 1];
    // var new_lat = last_waypoint.lat + 0.1 * Math.random() * (Math.random() > 0.9 ? -1 : 1);
    // var new_lng = last_waypoint.lng + 0.1 * Math.random() * (Math.random() > 0.9 ? -1 : 1);
    download_data()
    cmplt_path.push({lat: new_lat, lng: new_lng});
    lineCoordinatesPath.setPath(cmplt_path);
    map.setCenter({lat: new_lat, lng: new_lng})
    map_marker.setPosition({lat: new_lat, lng: new_lng});

}

function download_data() {
    $.ajax({
        url: "db_download.php?date="+time_line[time_line.length - 1],
        dataType: "text",
        success: function(data) {
            // alert(data);
            var new_data = $.parseJSON(data);
            new_data.forEach(function(pos) {
                cmplt_path.push({lat: pos[1], lng: pos[2]});
                time_line.push(new_data[0]);
            });
        }
            
    });
}