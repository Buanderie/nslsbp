/* Scripts used in map.html */
var gps_cmplt_path = new Array();
var xbee_cmplt_path = new Array();
var gps_time_line  = new Array();
var xbee_time_line  = new Array();
var hk_data    = new Array();
var xbee_data    = new Array();
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
        path: gps_cmplt_path,
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

    if(gps_cmplt_path.length > 0) {
        lineCoordinatesPath.setPath(gps_cmplt_path);
        map.setCenter({lat: gps_cmplt_path[gps_cmplt_path.length-1].lat, lng: gps_cmplt_path[gps_cmplt_path.length-1].lng});
        map_marker.setPosition({lat: gps_cmplt_path[gps_cmplt_path.length-1].lat, lng: gps_cmplt_path[gps_cmplt_path.length-1].lng});
    } else {
        console.log("NO GPS DATA");
        map.setCenter({lat: 41.39, lng: 2.11});
    }

    if(xbee_cmplt_path.length > 0) {
        lineCoordinatesPath.setPath(xbee_cmplt_path);
        // map.setCenter({lat: xbee_cmplt_path[xbee_cmplt_path.length-1].lat, lng: xbee_cmplt_path[xbee_cmplt_path.length-1].lng});
        // map_marker.setPosition({lat: xbee_cmplt_path[xbee_cmplt_path.length-1].lat, lng: xbee_cmplt_path[xbee_cmplt_path.length-1].lng});
    } else {
        console.log("NO XBEE DATA");
        // map.setCenter({lat: 41.39, lng: 2.11});
    }
}

/* GPS index definition */
const IDX_GPS_ROW_ID   = 0;
const IDX_GPS_TIME_SBC = 1;
const IDX_GPS_TIME_GPS = 2;
const IDX_GPS_LAT      = 3;
const IDX_GPS_LNG      = 4;
const IDX_GPS_GSPEED   = 5;
const IDX_GPS_SEA_ALT  = 6;
const IDX_GPS_GEO_ALT  = 7;
const IDX_GPS_CPU_TEMP = 8;
const IDX_GPS_GPU_TEMP = 9;

/* XBee index definition */
const IDX_XBEE_ROW_ID        = 0;
const IDX_XBEE_TIME_GPS      = 1;
const IDX_XBEE_LAT           = 2;
const IDX_XBEE_LNG           = 3;
const IDX_XBEE_ALT           = 4;
const IDX_XBEE_ROLL          = 5;
const IDX_XBEE_PITCH         = 6;
const IDX_XBEE_YAW           = 7;
const IDX_XBEE_VSYS          = 8;
const IDX_XBEE_ISYS          = 9;
const IDX_XBEE_WSYS          = 10;
const IDX_XBEE_OUT_TEMP      = 11;
const IDX_XBEE_GEN_TEMP      = 12;
const IDX_XBEE_PAY_TEMP      = 13;
const IDX_XBEE_BAT_TEMP      = 14;
const IDX_XBEE_TC_RECEIVED   = 15;
const IDX_XBEE_PING_RECEIVED = 16;

function download_data() {
    var path = "db_download.php?date_gps=" + (gps_time_line.length < 1 ? 0 : gps_time_line[gps_time_line.length - 1].time_gps) + "&date_xbee=" + (xbee_time_line.length < 1 ? 0 : xbee_time_line[xbee_time_line.length - 1]);
    $.ajax({
        url: path,
        dataType: "text",
        success: function(data) {
            var new_data = $.parseJSON(data);
            new_data.gps.forEach(function(query_data) {
                gps_cmplt_path.push({
                    lat: parseFloat(query_data[IDX_GPS_LAT]),
                    lng: parseFloat(query_data[IDX_GPS_LNG])
                });
                gps_time_line.push({
                    time_sbc: parseFloat(query_data[IDX_GPS_TIME_SBC]),
                    time_gps: parseFloat(query_data[IDX_GPS_TIME_GPS])
                });
                hk_data.push({
                    gspeed: parseFloat(query_data[IDX_GPS_GSPEED]),
                    sea_alt: parseFloat(query_data[IDX_GPS_SEA_ALT]),
                    geo_alt: parseFloat(query_data[IDX_GPS_GEO_ALT]),
                    cpu_temp: parseFloat(query_data[IDX_GPS_CPU_TEMP]),
                    gpu_temp: parseFloat(query_data[IDX_GPS_GPU_TEMP])
                });
            });
            new_data.xbee.forEach(function(query_data) {
                xbee_cmplt_path.push({
                    lat: parseFloat(query_data[IDX_XBEE_LAT]),
                    lng: parseFloat(query_data[IDX_XBEE_LNG])
                });
                xbee_time_line.push(parseFloat(query_data[IDX_XBEE_TIME_GPS]));
                xbee_data.push({
                    alt: parseFloat(query_data[IDX_XBEE_ALT]),
                    roll: parseFloat(query_data[IDX_XBEE_ROLL]),
                    pitch: parseFloat(query_data[IDX_XBEE_PITCH]),
                    yaw: parseFloat(query_data[IDX_XBEE_YAW]),
                    vsys: parseFloat(query_data[IDX_XBEE_VSYS]),
                    isys: parseFloat(query_data[IDX_XBEE_ISYS]),
                    wsys: parseFloat(query_data[IDX_XBEE_WSYS]),
                    out_temp: parseFloat(query_data[IDX_XBEE_OUT_TEMP]),
                    gen_temp: parseFloat(query_data[IDX_XBEE_GEN_TEMP]),
                    pay_temp: parseFloat(query_data[IDX_XBEE_PAY_TEMP]),
                    bat_temp: parseFloat(query_data[IDX_XBEE_BAT_TEMP]),
                    tc_received: parseFloat(query_data[IDX_XBEE_TC_RECEIVED]),
                    ping_received: parseFloat(query_data[IDX_XBEE_PING_RECEIVED])
                    
                });
            });
            if(new_data.gps.length > 0 || new_data.xbee.length > 0) {
                update_tables();
                redraw();
            }
        }

    });
}
