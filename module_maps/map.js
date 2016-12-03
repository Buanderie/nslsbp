/* Scripts used in map.html */
var cmplt_path = new Array();
var time_line  = new Array();
var hk_data    = new Array();
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
    console.log("NSLSBP map ready.");
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

    var interval = window.setInterval(download_data, 1000);
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

const IDX_ROW_ID        = 0;
const IDX_TIME_SBC      = 1;
const IDX_TIME_GPS      = 2;
const IDX_LAT           = 3;
const IDX_LNG           = 4;
const IDX_GSPEED        = 5;
const IDX_SEA_ALT       = 6;
const IDX_GEO_ALT       = 7;
const IDX_ACC_X         = 8;
const IDX_ACC_Y         = 9;
const IDX_ACC_Z         = 10;
const IDX_GYRO_X        = 11;
const IDX_GYRO_Y        = 12;
const IDX_GYRO_Z        = 13;
const IDX_MAG_X         = 14;
const IDX_MAG_Y         = 15;
const IDX_MAG_Z         = 16;
const IDX_CPU_TEMP      = 17;
const IDX_GPU_TEMP      = 18;
const IDX_IN_TEMP       = 19;
const IDX_IN_PRESSURE   = 20;
const IDX_OUT_TEMP      = 21;
const IDX_OUT_PRESSURE  = 22;

function download_data() {
    $.ajax({
        url: "db_download.php?date=" + (time_line.length < 1 ? 0 : time_line[time_line.length - 1].gps),
        dataType: "text",
        success: function(data) {
            var new_data = $.parseJSON(data);
            new_data.forEach(function(query_data) {
                cmplt_path.push({
                    lat: parseFloat(query_data[IDX_LAT]),
                    lng: parseFloat(query_data[IDX_LNG])
                });
                time_line.push({
                    time_sbc: parseFloat(query_data[IDX_TIME_SBC]),
                    time_gps: parseFloat(query_data[IDX_TIME_GPS])
                });
                hk_data.push({
                    gspeed: parseFloat(query_data[IDX_GSPEED]),
                    sea_alt: parseFloat(query_data[IDX_SEA_ALT]),
                    geo_alt: parseFloat(query_data[IDX_GEO_ALT]),
                    acc_x: parseFloat(query_data[IDX_ACC_X]),
                    acc_y: parseFloat(query_data[IDX_ACC_Y]),
                    acc_z: parseFloat(query_data[IDX_ACC_Z]),
                    gyro_x: parseFloat(query_data[IDX_GYRO_X]),
                    gyro_y: parseFloat(query_data[IDX_GYRO_Y]),
                    gyro_z: parseFloat(query_data[IDX_GYRO_Z]),
                    mag_x: parseFloat(query_data[IDX_MAG_X]),
                    mag_y: parseFloat(query_data[IDX_MAG_Y]),
                    mag_z: parseFloat(query_data[IDX_MAG_Z]),
                    cpu_temp: parseFloat(query_data[IDX_CPU_TEMP]),
                    gpu_temp: parseFloat(query_data[IDX_GPU_TEMP]),
                    in_temp: parseFloat(query_data[IDX_IN_TEMP]),
                    in_pressure: parseFloat(query_data[IDX_IN_PRESSURE]),
                    out_temp: parseFloat(query_data[IDX_OUT_TEMP]),
                    out_pressure: parseFloat(query_data[IDX_OUT_PRESSURE])
                });
            });
            if(new_data.length > 0){ redraw(); }
        }

    });
}
