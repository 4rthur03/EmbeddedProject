<html>
<head>
<title>BeagleBone Rlay Control</title>
<style type="text/css">
	p {display: table-cell;}
	button {width: 75px; margin: 2px auto;}
</style>

<?php
	if (isset($_GET['onOff']))
	{
		$onOff=$_GET['onOff'];
		
		echo exec("/var/www/cgi-bin/setrelay $onOff" );
	}
?>
</head>
<body>
<div style="width: 200px; margin:0px autp;">
	<div style="width: 100px; float: left;">
		<p>Electrical Appliance</p>
		<button type="button" onclick="location.href='relayCtl.php?onOff=1'">ON</button>

		<button type="button" onclick="location.href='relayCtl.php?onOff=0'">OFF</button>
	</div>
</div>
</body>
</html>

