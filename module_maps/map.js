/* Scripts used in map.html */
var gps_cmplt_path = new Array();
var xbee_cmplt_path = new Array();
var gps_time_line  = new Array();
var xbee_time_line  = new Array();
// var cmplt_path = [
//             {lat: 37.772, lng: -122.214},
//             {lat: 21.291, lng: -157.821},
//             {lat: -18.142, lng: 178.431},
//             {lat: -27.467, lng: 153.027}
// ];
// var time_line = [1474022404, 1474022405, 1474022406, 1474022407];
var lineCoordinatesPathGPS, lineCoordinatesPathXBee;
var map;
var map_marker;
var interval_tables, interval_map;
var usr_time;
var usr_recenter = true;

/* On document ready actions: ------------------------------------------------------------------ */
$(document).ready(function() {
    //initMap();
    console.log("NSLSBP map ready.");
    //update_tables();
    interval_tables = window.setInterval(update_tables, 1000);

    var usr_date = top.document.getElementById('gps_frame').contentWindow.document.getElementById('usr_date');
    var usr_all = top.document.getElementById('gps_frame').contentWindow.document.getElementById('usr_all');
    var usr_recenter_cb = top.document.getElementById('gps_frame').contentWindow.document.getElementById('usr_recenter_cb');
    var nowos = new Date();
    nowos.setDate(nowos.getDate() - 1);
    var day_str   = (nowos.getDate() < 10 ? "0" : "") + nowos.getDate();
    var hour_str  = (nowos.getHours() < 10 ? "0" : "") + nowos.getHours();
    var min_str   = (nowos.getMinutes() < 10 ? "0" : "") + nowos.getMinutes();
    var month_str = ((nowos.getMonth() + 1) < 10 ? "0" : "") + (nowos.getMonth() + 1);
    var nowos_str = nowos.getFullYear() + "-" + month_str + "-" + day_str + "T" + hour_str + ":" + min_str + ":00.000";
    console.log("Retrieving points from: " + nowos_str);
    usr_date.value = nowos_str;

    /* Install onInput listener: */
    usr_date.oninput = usr_date_changed;
    usr_all.onchange = usr_date_changed;
    usr_recenter_cb.onchange = usr_recenter_changed;

    /* Set loading timestamp: */
    usr_time = (nowos.getTime() / 1000).toFixed();
    usr_recenter = usr_recenter_cb.checked;
});


function usr_date_changed()
{
    console.log("User date settings chaged");
    var usr_date = top.document.getElementById('gps_frame').contentWindow.document.getElementById('usr_date');
    var usr_all = top.document.getElementById('gps_frame').contentWindow.document.getElementById('usr_all');
    if(usr_all.checked) {
        usr_time = 0;
        gps_cmplt_path = new Array();
        xbee_cmplt_path = new Array();
        gps_time_line  = new Array();
        xbee_time_line = new Array();
        download_data();
    } else {
        var nowos = new Date(usr_date.value);
        nowos.setDate(nowos.getDate() - 1);
        var day_str   = (nowos.getDate() < 10 ? "0" : "") + nowos.getDate();
        var hour_str  = (nowos.getHours() < 10 ? "0" : "") + nowos.getHours();
        var min_str   = (nowos.getMinutes() < 10 ? "0" : "") + nowos.getMinutes();
        var month_str = ((nowos.getMonth() + 1) < 10 ? "0" : "") + (nowos.getMonth() + 1);
        var nowos_str = nowos.getFullYear() + "-" + month_str + "-" + day_str + "T" + hour_str + ":" + min_str + ":00.000";
        console.log("Retrieving points from: " + nowos_str);
        usr_time = (nowos.getTime() / 1000).toFixed();
        gps_cmplt_path = new Array();
        xbee_cmplt_path = new Array();
        gps_time_line  = new Array();
        xbee_time_line = new Array();
        download_data();
    }
}


function usr_recenter_changed()
{
    console.log("User map recenter settings chaged");
    var usr_recenter_cb = top.document.getElementById('gps_frame').contentWindow.document.getElementById('usr_recenter_cb');
    usr_recenter = usr_recenter_cb.checked;
    download_data();
}


function initMap() {
    map = new google.maps.Map(document.getElementById('map'), {
        center: {lat: 41.39, lng: 2.11},
        zoom: 12
    });
    // Create the legend and display on the map
    var legend = document.createElement('div');
    legend.id = 'legend';
    var content = [];
    content.push('<h3>MAP DATA</h3>');
    content.push('<p><div class="color blue"></div>GPS</p>');
    content.push('<p><div class="color red"></div>XBEE</p>');
    legend.innerHTML = content.join('');
    legend.index = 1;
    map.controls[google.maps.ControlPosition.RIGHT_TOP].push(legend);

    lineCoordinatesPathGPS = new google.maps.Polyline({
        path: gps_cmplt_path,
        geodesic: true,
        strokeColor: '#2E10FF',
        strokeOpacity: 1.0,
        strokeWeight: 2
    });
    lineCoordinatesPathXBee = new google.maps.Polyline({
        path: xbee_cmplt_path,
        geodesic: true,
        strokeColor: '#B5192C',
        strokeOpacity: 1.0,
        strokeWeight: 2
    });
    map_marker = new google.maps.Marker({position: {lat: 41.39, lng: 2.11}, map: map});
    map_marker.setMap(map);
    
    lineCoordinatesPathGPS.setMap(map);
    lineCoordinatesPathXBee.setMap(map);

    interval_map = window.setInterval(download_data, 1000);
}
function redraw() {
    // var waypoints = lineCoordinatesPath.getPath();
    // var last_waypoint = waypoints.getAt(waypoints.getLength() - 1);
    // var new_lat = last_waypoint.lat() + 0.01;
    // var new_lng = last_waypoint.lng() + 0.01;
    // waypoints.push(new google.maps.LatLng(new_lat, new_lng));
    // map.setCenter({lat: new_lat, lng: new_lng})
    // map_marker.setPosition({lat: new_lat, lng: new_lng});

    // if(gps_cmplt_path.length > 0 && xbee_cmplt_path.length == 0) {
    //     lineCoordinatesPath.setPath(gps_cmplt_path);
    //     if(usr_recenter) {
    //         map.setCenter({lat: gps_cmplt_path[gps_cmplt_path.length-1].lat, lng: gps_cmplt_path[gps_cmplt_path.length-1].lng});
    //         map_marker.setPosition({lat: gps_cmplt_path[gps_cmplt_path.length-1].lat, lng: gps_cmplt_path[gps_cmplt_path.length-1].lng});
    //     }
    // }

    // if(xbee_cmplt_path.length > 0) {
    //     lineCoordinatesPath.setPath(xbee_cmplt_path);
    //     if(usr_recenter) {
    //         map.setCenter({lat: xbee_cmplt_path[xbee_cmplt_path.length-1].lat, lng: xbee_cmplt_path[xbee_cmplt_path.length-1].lng});
    //         map_marker.setPosition({lat: xbee_cmplt_path[xbee_cmplt_path.length-1].lat, lng: xbee_cmplt_path[xbee_cmplt_path.length-1].lng});
    //     }
    // }

    if(xbee_cmplt_path.length > 0) {
        lineCoordinatesPathXBee.setPath(xbee_cmplt_path);
    } else {
        console.log("NO XBEE DATA");
    }
    if(gps_cmplt_path.length > 0) {
        lineCoordinatesPathGPS.setPath(gps_cmplt_path);
        map.setCenter({lat: gps_cmplt_path[gps_cmplt_path.length-1].lat, lng: gps_cmplt_path[gps_cmplt_path.length-1].lng});
        map_marker.setPosition({lat: gps_cmplt_path[gps_cmplt_path.length-1].lat, lng: gps_cmplt_path[gps_cmplt_path.length-1].lng});
        if(usr_recenter) {
            map.setCenter({lat: gps_cmplt_path[gps_cmplt_path.length-1].lat, lng: gps_cmplt_path[gps_cmplt_path.length-1].lng});
            map_marker.setPosition({lat: gps_cmplt_path[gps_cmplt_path.length-1].lat, lng: gps_cmplt_path[gps_cmplt_path.length-1].lng});
        }
    } else {
        console.log("NO GPS DATA");
        map.setCenter({lat: 41.39, lng: 2.11});
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
    var path =  "map_download.php?date_gps=" + (gps_time_line.length < 1 ? usr_time : gps_time_line[gps_time_line.length - 1].time_gps) +
                "&date_xbee=" + (xbee_time_line.length < 1 ? usr_time : xbee_time_line[xbee_time_line.length - 1]);
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
            });
            new_data.xbee.forEach(function(query_data) {
                xbee_cmplt_path.push({
                    lat: parseFloat(query_data[IDX_XBEE_LAT]),
                    lng: parseFloat(query_data[IDX_XBEE_LNG])
                });
                xbee_time_line.push(parseFloat(query_data[IDX_XBEE_TIME_GPS]));
            });
            redraw();
        }

    });
}
