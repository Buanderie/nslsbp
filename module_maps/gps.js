/* Scripts used in gps.html (or map.html) */
// var last_date = 0;
var time_interval = null;
var time_diff = 0;

function update_gps_tables() {
    var cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_rx_time');
    cell.innerHTML = (new Date(time_line[time_line.length-1].rx*1000)).toLocaleString();

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_gps_time');
    cell.innerHTML = (new Date(time_line[time_line.length-1].gps*1000)).toLocaleString();

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_lat');
    cell.innerHTML = cmplt_path[cmplt_path.length-1].lat;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_lng');
    cell.innerHTML = cmplt_path[cmplt_path.length-1].lng;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_vel');
    cell.innerHTML = gps_extra[gps_extra.length-1].kph;
    
    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_temp');
    cell.innerHTML = gps_extra[gps_extra.length-1].temp;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_sea');
    cell.innerHTML = gps_extra[gps_extra.length-1].sea;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_geo');
    cell.innerHTML = gps_extra[gps_extra.length-1].geo;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_course');
    cell.innerHTML = gps_extra[gps_extra.length-1].course;

    time_diff = Math.floor((Date.now()/1000) - time_line[time_line.length-1].rx);
    time_update();

}

function time_update() {
    var cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_update');
    if(time_diff < 0) {
        console.log('Error on last update (future time)');
        cell.innerHTML = "? seconds";
        time_interval = null;
    } else {
        if(time_diff < 60) {
            cell.innerHTML = time_diff + " second(s)";
            if(time_interval != null) clearInterval(time_interval);
            time_interval = setInterval(time_update, 20*1000);
        } else {
            time_diff = Math.floor(time_diff/60);
            if(time_diff < 60) {
                cell.innerHTML = time_diff + " minute(s)";
                if(time_interval != null) clearInterval(time_interval);
                time_interval = setInterval(time_update, 60*1000);
            } else {
                time_diff = Math.floor(time_diff/60);
                if(time_diff < 24) {
                    cell.innerHTML = time_diff + " hour(s)";
                    if(time_interval != null) clearInterval(time_interval);
                    time_interval = setInterval(time_update, 3600*1000);
                } else {
                    time_diff = Math.floor(time_diff/24);
                    cell.innerHTML = time_diff + " day(s)";
                    if(time_interval != null) clearInterval(time_interval);
                    time_interval = setInterval(time_update, 3600*1000);
                }
            }
        }
    }
}