/* Scripts used in map.html */
var last_date = 0;
var cmplt_path = new Array();
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
        path: flightPlanCoordinates,
        geodesic: true,
        strokeColor: '#2E10FF',
        strokeOpacity: 1.0,
        strokeWeight: 2
    });
    map_marker = new google.maps.Marker({position: {lat: -27.467, lng: 153.027}, map: map});

    map_marker.setMap(map);
    lineCoordinatesPath.setMap(map);

    var interval = window.setInterval(redraw, 1000);
}
function redraw() {
    // var waypoints = lineCoordinatesPath.getPath();
    // var last_waypoint = waypoints.getAt(waypoints.getLength() - 1);
    // var new_lat = last_waypoint.lat() + 0.01;
    // var new_lng = last_waypoint.lng() + 0.01;
    // waypoints.push(new google.maps.LatLng(new_lat, new_lng));
    // map.setCenter({lat: new_lat, lng: new_lng})
    // map_marker.setPosition({lat: new_lat, lng: new_lng});


    var last_waypoint = flightPlanCoordinates[flightPlanCoordinates.length - 1];
    var new_lat = last_waypoint.lat + 0.1 * Math.random() * (Math.random() > 0.9 ? -1 : 1);
    var new_lng = last_waypoint.lng + 0.1 * Math.random() * (Math.random() > 0.9 ? -1 : 1);
    flightPlanCoordinates.push({lat: new_lat, lng: new_lng});
    lineCoordinatesPath.setPath(flightPlanCoordinates);
    map.setCenter({lat: new_lat, lng: new_lng})
    map_marker.setPosition({lat: new_lat, lng: new_lng});

}

function download_data() {
    $.ajax({
        url: "manage_queue.php?op=get",
        dataType: "text",
        success: function(data) {
            var new_data = $.parseJSON(data);
            for each (var pos in new_data) {
                cmplt_path.push({lat: pos[0], lng: pos[1]});
            }
        }
            
    });
}