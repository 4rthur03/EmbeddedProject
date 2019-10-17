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


