t <html><head><title>RTC Input</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("rtc.cgx", 500);
t function plotADGraph() {
t  timeVal = document.getElementById("HORA").value;
t  dateVal = document.getElementById("FECHA").value;
t  document.getElementById("HORA").value = (timeVal);
t  document.getElementById("FECHA").value = (dateVal);
t }
t function periodicUpdateAd() {
t  if(document.getElementById("RTCChkBox").checked == true) {
t   updateMultiple(formUpdate,plotADGraph);
t   rtc_elTime = setTimeout(periodicUpdateAd, formUpdate.period);
t  }
t  else
t   clearTimeout(rtc_elTime);
t }
t </script></head>
i pg_header.inc
t <h2 align="center"><br>RTC Input</h2>
t <p><font size="2">This page allows you to monitor RTC input value to know
t  the actual hour and date. Periodic screen update is based on <b>xml</b> 
t  technology.This results in smooth flicker-free screen update.<br><br>
t <form action="rtc.cgi" method="post" name="rtc">
t <input type="hidden" value="rtc" name="rtc">
t <table border=0 width=99%><font size="3">
t <tr style="background-color: #aaccff">
t <th width=35%>HORA</th> 
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
c h 1  size="18" id="rtc_horas" value="%s"></td>
t </tr>
t <table border=0 width=99%><font size="3">
t <tr style="background-color: #aaccff">
t <th width=35%>FECHA</th> 
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
c h 2  size="20" id="rtc_fecha" value="%s"></td>
t </font></table>
t <p align=center>
t  <input type=button value="Refresh" onclick="location='/rtc.cgi'">
t  Periodic:<input type="checkbox" id="RTCChkBox" onclick="periodicUpdateAd()">
t </p></form>
. End of script must be closed with period


