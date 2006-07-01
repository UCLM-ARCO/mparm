#!/bin/sh

grep "Processor" | gawk --non-decimal-data '
BEGIN {
  print "Type\tMin\tAverage\tMax\tStdDev"
}
{
  if ($3 == "shuts") {
  }
  else if ($4 == "Test") {
    if ($5 != "MPS" && $2 == 1) {
      printf $5 "\t"
      sqr = 0
      sum = 0
      max = -10000000
      min =  10000000
      for (i=0 ; i<N; ++i) {
        d = (p1e[i] - p0e[i])
        sqr = sqr + (d*d)
        sum = sum + d
        if (d < min) min = d
        if (d > max) max = d
        if (p1b[i] > p0e[i])
          print "ERROR. There was no blocking in this case!"
        # print p0b[i] "\t" p0e[i] "\t" p1b[i] "\t" p1e[i] "\t" d
      }
      avg = sum / N
      stddev = sqrt(sqr) / sum
      print min "\t" avg "\t" max "\t" stddev
    }
  }
  else if ($2 == 0) {
    i = 0 + $4
    if (N <= i) N = i + 1
    if ($5 == "start_time:")
      p0b[i] = $6
    if ($5 == "stop_time:")
      p0e[i] = $6
  }
  else if ($2 == 1) {
    i = 0 + $4
    if ($5 == "start_time:")
      p1b[i] = $6
    if ($5 == "stop_time:")
      p1e[i] = $6
  }
}
' | sed -e 's/://g' -e 's/Latency//g' -e 's/^Sh/ShMem::/g' -e 's/^Scr/ScrMem::/g'