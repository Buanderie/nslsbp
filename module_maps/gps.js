/* Scripts used in gps.html (or map.html) */
// var last_date = 0;
var time_interval = null;
var time_diff = 0;

function update_tables() {
    var path = "tables_download.php";
    $.ajax({
        url: path,
        dataType: "text",
        success: function(data) {
            var new_data = $.parseJSON(data);

            var cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_sbc_time');
            cell.innerHTML = (new Date(new_data.gps.time_local*1000)).toLocaleString();

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_gps_time');
            cell.innerHTML = (new Date(new_data.gps.time_gps*1000)).toLocaleString();

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_lat');
            cell.innerHTML = new_data.gps.lat;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_lng');
            cell.innerHTML = new_data.gps.lng;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_gspeed');
            cell.innerHTML = new_data.gps.gspeed;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_sea_alt');
            cell.innerHTML = new_data.gps.sea_alt / 1000;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_geo_alt');
            cell.innerHTML = new_data.gps.geo_alt / 1000;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_xbee_gps_time');
            cell.innerHTML = (new Date(new_data.xbee.time_gps*1000)).toLocaleString();

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_xbee_lat');
            cell.innerHTML = new_data.xbee.lat;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_xbee_lng');
            cell.innerHTML = new_data.xbee.lng;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_xbee_alt');
            cell.innerHTML = new_data.xbee.alt;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_sys_volt');
            cell.innerHTML = new_data.xbee.vsys;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_sys_curr');
            cell.innerHTML = new_data.xbee.isys;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_sys_power');
            cell.innerHTML = new_data.xbee.wsys;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_temp_out');
            cell.innerHTML = new_data.xbee.out_temp;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_temp_gen');
            cell.innerHTML = new_data.xbee.gen_temp;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_temp_pay');
            cell.innerHTML = new_data.xbee.pay_temp;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_temp_bat');
            cell.innerHTML = new_data.xbee.bat_temp;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_roll');
            cell.innerHTML = new_data.xbee.roll;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_pitch');
            cell.innerHTML = new_data.xbee.pitch;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_yaw');
            cell.innerHTML = new_data.xbee.yaw;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_cpu_temp');
            cell.innerHTML = new_data.gps.cpu_temp;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_gpu_temp');
            cell.innerHTML = new_data.gps.gpu_temp;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_tc_received');
            cell.innerHTML = new_data.xbee.tc_received;

            cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('param_ping_received');
            cell.innerHTML = new_data.xbee.ping_received;
            
        }

    });
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
