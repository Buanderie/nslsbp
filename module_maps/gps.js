/* Scripts used in gps.html (or map.html) */
// var last_date = 0;

function update_gps_tables() {
    var cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('gps_time');
    cell.innerHTML = (new Date(time_line[time_line.length-1]*1000)).toLocaleString();

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('gps_lat');
    cell.innerHTML = cmplt_path[cmplt_path.length-1].lat;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('gps_lng');
    cell.innerHTML = cmplt_path[cmplt_path.length-1].lng;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('gps_kph');
    cell.innerHTML = gps_extra[gps_extra.length-1].kph;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('gps_sea');
    cell.innerHTML = gps_extra[gps_extra.length-1].sea;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('gps_geo');
    cell.innerHTML = gps_extra[gps_extra.length-1].geo;

    cell = top.document.getElementById('gps_frame').contentWindow.document.getElementById('gps_course');
    cell.innerHTML = gps_extra[gps_extra.length-1].course;

    // var tab = top.document.getElementById('gps_frame').contentWindow.document.getElementById('gps_tab');
    // var row = document.createElement("div");
    // row.setAttribute("class", "table_row");

    // var cell = document.createElement("div");
    // cell.setAttribute("class","table_cell");
    // cell.setAttribute("id","gps_time_"+time_line[time_line.length-1]);
    // cell.innerHTML = (new Date(time_line[time_line.length-1]*1000)).toLocaleString();
    // row.appendChild(cell);

    // cell = document.createElement("div");
    // cell.setAttribute("class","table_cell");
    // cell.setAttribute("id","gps_lat_"+time_line[time_line.length-1]);
    // cell.innerHTML = cmplt_path[cmplt_path.length-1].lat;
    // row.appendChild(cell);

    // cell = document.createElement("div");
    // cell.setAttribute("class","table_cell");
    // cell.setAttribute("id","gps_lng_"+time_line[time_line.length-1]);
    // cell.innerHTML = cmplt_path[cmplt_path.length-1].lng;
    // row.appendChild(cell);

    // tab.appendChild(row);


}