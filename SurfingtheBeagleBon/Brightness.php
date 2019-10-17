<html>
<head>
	<title>BeagleBone Brightness</title>
    <script type="text/javascript" src="https://www.google.com/jsapi"></script>
    <script type="text/javascript">
      google.load("visualization", "1", {packages:["corechart"]});
      google.setOnLoadCallback(drawChart);
      function drawChart() {
        var data = google.visualization.arrayToDataTable([
			['Time', 'Brightness'],
			<?php
				$con = mysqli_connect("localhost", "user", "user", "BrightnessDB");
				
				$query = "SELECT * FROM BrightData";
				$result = mysqli_query($con, $query);
				
				mysqli_close($con);
				
				while ($row = mysqli_fetch_array($result))
				{
					$time = $row['BrightTime'];
					$bright = $row['Brightness'];
					echo "['$time', $bright],";
				}
			?>
        ]);

        var options = {
          title: 'BeagleBone Measured Brightness',
		  vAxis: { title: "Brightness" }
        };

        var chart = new google.visualization.LineChart(document.getElementById('chart_div'));
        chart.draw(data, options);
      }
    </script>
</head>
<body>
	<div id="chart_div" style="width: 900px; height: 500px;"></div>
</body>
</html>