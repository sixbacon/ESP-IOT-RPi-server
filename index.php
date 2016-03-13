<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">  
<html xmlns="http://www.w3.org/1999/xhtml">                                                                                
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
<title>Lincoln Hill Data</title>
<style type="text/css">
<!--
body {
	background-image: url(images/CIMG0040.JPG);
}
-->
</style></head>

<body>  
<table width="700" border="0" cellpadding="0" cellspacing="0" bordercolor="#6699CC">
  <tr>
    <td><?php

$dbcnx = @mysql_connect('localhost', 'bill', 'bandband');  		// connect to mysql
if (!$dbcnx)  {
  echo '<p>Unable to connect to mysql</p>';
  exit();
}  

if (!@mysql_select_db('PVgriddb'))   {
  exit('<p>Unable to connect to database</p>');
}									//connect to database
$result = @mysql_query('SELECT * FROM data WHERE (DATE(timestamp) = DATE(NOW())) AND (source ="ESP178/PVWh")');				// issue a query
if (!$result)  {
  exit('<p>Query error</p>');
}

$myFile = "datapv.txt";							// write data to a text file for gnuplot
@unlink($myFile); 							// delete any previous version @ suppresses errors

$fh = fopen($myFile, 'w');						// create file for writing

while ($row = mysql_fetch_array($result))  {
  fwrite($fh,$row['timestamp']." ".$row['count']."\n");			//write data
}

fclose($fh);								// orderly close

exec('/usr/bin/gnuplot /var/www/datapv.plt');                           // hand to gnuplot to graph using script datapv.plt, which knows data is in datapv.txt
				
// now do for grid info

$result = @mysql_query('SELECT * FROM data WHERE (DATE(timestamp) = DATE(NOW())) AND (source ="ESP178/gridWh")');				// issue a query
if (!$result)  {
  exit('<p>Query error</p>');
}

$myFile = "datagrid.txt";						// write data to a text file for gnuplot
@unlink($myFile); 							// delete any previous version @ suppresses errors

$fh = fopen($myFile, 'w');						// create file for writing

while ($row = mysql_fetch_array($result))  {
  fwrite($fh,$row['timestamp']." ".$row['count']."\n");			//write data
}

fclose($fh);								// orderly close

exec('/usr/bin/gnuplot /var/www/datagrid.plt');                         // hand to gnuplot to graph using script datagrid.plt, which knows data is in datagrid.txt


$result = @mysql_query('SELECT * FROM data WHERE (DATE(timestamp) = DATE(NOW())) AND (source ="ESP178/gridWh")');				// issue a query
if (!$result)  {
  exit('<p>Query error</p>');
}

$myFile = "datagrid.txt";							// write data to a text file for gnuplot
@unlink($myFile); 							// delete any previous version @ suppresses errors

$fh = fopen($myFile, 'w');						// create file for writing

while ($row = mysql_fetch_array($result))  {
  fwrite($fh,$row['timestamp']." ".$row['count']."\n");			//write data
}

fclose($fh);								// orderly close

exec('/usr/bin/gnuplot /var/www/datagrid.plt');
				// call gnupolt with script file
#while ($row = mysql_fetch_array($result))  {
# echo "{$row['timestamp']}   {$row['count']} <br />";    //write data 
#}

?></td>
  </tr>
</table>
<img src = "datapv.png" style = "float:left;width:48%;margin-right:1%;margin-botton:0.5em;">
<img src = "datagrid.png" style = "float:left;width:48%;margin-right:1%;margin-botton:0.5em;">

</body>  
</html>
