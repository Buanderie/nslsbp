/* Scripts used in gps.html (or map.html) */
// var last_date = 0;
var time_interval = null;
var time_diff = 0;

function update_tables() {
    var cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_sbc_time');
    cell.innerHTML = (new Date(gps_time_line[gps_time_line.length-1].time_sbc*1000)).toLocaleString();

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_gps_time');
    cell.innerHTML = (new Date(gps_time_line[gps_time_line.length-1].time_gps*1000)).toLocaleString();

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_lat');
    cell.innerHTML = gps_cmplt_path[gps_cmplt_path.length-1].lat;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_lng');
    cell.innerHTML = gps_cmplt_path[gps_cmplt_path.length-1].lng;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_gspeed');
    cell.innerHTML = hk_data[hk_data.length-1].gspeed;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_sea_alt');
    cell.innerHTML = hk_data[hk_data.length-1].sea_alt / 1000;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_geo_alt');
    cell.innerHTML = hk_data[hk_data.length-1].geo_alt / 1000;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_xbee_gps_time');
    cell.innerHTML = (new Date(xbee_time_line[xbee_time_line.length-1]*1000)).toLocaleString();

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_xbee_lat');
    cell.innerHTML = xbee_cmplt_path[xbee_cmplt_path.length-1].lat;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_xbee_lng');
    cell.innerHTML = xbee_cmplt_path[xbee_cmplt_path.length-1].lng;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_xbee_alt');
    cell.innerHTML = xbee_data[xbee_data.length-1].alt;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_sys_volt');
    cell.innerHTML = xbee_data[xbee_data.length-1].vsys;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_sys_curr');
    cell.innerHTML = xbee_data[xbee_data.length-1].isys;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_sys_power');
    cell.innerHTML = xbee_data[xbee_data.length-1].wsys;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_temp_out');
    cell.innerHTML = xbee_data[xbee_data.length-1].out_temp;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_temp_gen');
    cell.innerHTML = xbee_data[xbee_data.length-1].gen_temp;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_temp_pay');
    cell.innerHTML = xbee_data[xbee_data.length-1].pay_temp;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_temp_bat');
    cell.innerHTML = xbee_data[xbee_data.length-1].bat_temp;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_roll');
    cell.innerHTML = xbee_data[xbee_data.length-1].roll;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_pitch');
    cell.innerHTML = xbee_data[xbee_data.length-1].pitch;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_yaw');
    cell.innerHTML = xbee_data[xbee_data.length-1].yaw;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_cpu_temp');
    cell.innerHTML = hk_data[hk_data.length-1].cpu_temp;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_gpu_temp');
    cell.innerHTML = hk_data[hk_data.length-1].gpu_temp;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_tc_received');
    cell.innerHTML = xbee_data[xbee_data.length-1].tc_received;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_ping_received');
    cell.innerHTML = xbee_data[xbee_data.length-1].ping_received;

    // time_diff = Math.floor((Date.now()/1000) - time_line[time_line.length-1].rx);
    // time_update();

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
