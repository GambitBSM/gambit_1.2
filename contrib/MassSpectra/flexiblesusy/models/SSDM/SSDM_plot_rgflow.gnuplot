set terminal x11

set title "SSDM renormalization group flow"
set xlabel "renormalization scale / GeV"
set logscale x

if (!exists("filename")) filename='SSDM_rgflow.dat'

plot for [i=2:36+1] filename using 1:(column(i)) title columnhead(i)
