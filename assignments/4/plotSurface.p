filenameWrite = 'singleFileRunWrite.dat'
filenameRead = 'singleFileRunWrite.dat'
set multiplot layout 2, 1 title "AMOS Run-time Performance Data for 16 File Parameter Sweep" font ",14"
set tmargin 2
set xr [0:8192]
set yr [0:16]
set zr [0:320]

set ticslevel 0

set xtics 
set ytics offset -0.5,-0.5
set ztics 

set title "Write Operation Performance"
set xlabel "Node Count (n)" rotate parallel offset 0,-1
set ylabel "Block Size (KB)" rotate parallel offset 0,0
set zlabel "Bandwidth (GB/s)" rotate parallel offset 0,0
unset key

#splot filenameWrite u 1:2:5 with points linetype 6

set dgrid3d 10, 10
set hidden3d


splot filenameWrite u 1:2:5 with lines linetype 6 



set xr [0:8192]
set yr [0:16]
set zr [0:320]
set title "Read Operation Performance"
set xlabel "Node Count (n)" rotate parallel offset 0,-1
set ylabel "Block Size (KB)" rotate parallel offset 0,0
set zlabel "Bandwidth (GB/s)" rotate parallel offset 0,0

set dgrid3d 10, 10
set hidden3d
unset key
splot filenameRead u 1:2:6 with lines linetype 7

#7 = RED
#6 = BLUE
