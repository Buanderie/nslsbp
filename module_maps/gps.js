/* Scripts used in gps.html (or map.html) */
// var last_date = 0;
var time_interval = null;
var time_diff = 0;

function update_gps_tables() {
    var cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_sbc_time');
    cell.innerHTML = (new Date(time_line[time_line.length-1].time_sbc*1000)).toLocaleString();

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_gps_time');
    cell.innerHTML = (new Date(time_line[time_line.length-1].time_gps*1000)).toLocaleString();

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_lat');
    cell.innerHTML = cmplt_path[cmplt_path.length-1].lat;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_lng');
    cell.innerHTML = cmplt_path[cmplt_path.length-1].lng;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_gspeed');
    cell.innerHTML = hk_data[hk_data.length-1].gspeed;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_sea_alt');
    cell.innerHTML = hk_data[hk_data.length-1].sea_alt / 1000;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_geo_alt');
    cell.innerHTML = hk_data[hk_data.length-1].geo_alt / 1000;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_cpu_temp');
    cell.innerHTML = hk_data[hk_data.length-1].cpu_temp;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_gpu_temp');
    cell.innerHTML = hk_data[hk_data.length-1].gpu_temp;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_in_temp');
    cell.innerHTML = hk_data[hk_data.length-1].in_temp;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_in_pressure');
    cell.innerHTML = hk_data[hk_data.length-1].in_pressure;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_out_temp');
    cell.innerHTML = hk_data[hk_data.length-1].out_temp;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_out_pressure');
    cell.innerHTML = hk_data[hk_data.length-1].out_pressure;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_acc');
    cell.innerHTML = hk_data[hk_data.length-1].acc_x + ", " + hk_data[hk_data.length-1].acc_y + ", " + hk_data[hk_data.length-1].acc_z;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_gyro');
    cell.innerHTML = hk_data[hk_data.length-1].gyro_x + ", " + hk_data[hk_data.length-1].gyro_y + ", " + hk_data[hk_data.length-1].gyro_z;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_mag');
    cell.innerHTML = hk_data[hk_data.length-1].mag_x + ", " + hk_data[hk_data.length-1].mag_y + ", " + hk_data[hk_data.length-1].mag_z;

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
