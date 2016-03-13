set terminal png small size 640 400
set output 'datapv.png'
set xtics rotate
set yrange [0:30]
set xdata time
set timefmt "%Y-%m-%d %H:%M:%S"
set ytics
set grid
set ylabel "1 min count"
set xtics font "Times-Roman, 8"
set format x "%Y/%m/%d\n%H:%M"
set xlabel "\nTime\n"
set key above
plot '/var/www/datapv.txt'  using 1:3 title "Generation Meter Readings" 
set output
