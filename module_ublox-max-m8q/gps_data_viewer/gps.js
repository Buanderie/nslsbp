/* Scripts used in gps.html (or map.html) */
// var last_date = 0;

function update_gps_tables() {
    var cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('gps_time');
    cell.innerHTML = (new Date(time_line[time_line.length-1]*1000)).toLocaleString();

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('gps_lat');
    cell.innerHTML = cmplt_path[cmplt_path.length-1].lat;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('gps_lng');
    cell.innerHTML = cmplt_path[cmplt_path.length-1].lng;
}