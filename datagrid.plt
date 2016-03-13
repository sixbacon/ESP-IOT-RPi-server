set terminal png small size 640 400
set output 'datagrid.png'
set xtics rotate
set yrange [0:100]
set xdata time
set timefmt "%Y-%m-%d %H:%M:%S"
set ytics
set grid
set ylabel "1 min count"
set xtics font "Times-Roman, 8"
set format x "%Y/%m/%d\n%H:%M"
set xlabel "\nTime\n"
set key above
plot '/var/www/datagrid.txt'  using 1:3 title "Grid Meter Readings" 
set output
