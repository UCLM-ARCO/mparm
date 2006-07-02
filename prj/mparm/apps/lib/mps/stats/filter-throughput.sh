#!/bin/sh

grep "Processor" | gawk --non-decimal-data '
BEGIN {
  print "Size\tThroughput"
}
{
  if ($3 == "shuts") {
  }
  else if ($4 == "Test") {
    if ($5 != "MPS" && $2 == 0) {
      print sz "\t" 65536000 / (stop - start)
    }
  }
  else {
    sz = 0 + $4
    if ($5 == "start_time:")
      start = $6
    if ($5 == "stop_time:")
      stop = $6
  }
}
' | sed -e 's/://g' -e 's/Latency//g' -e 's/^Sh/ShMem::/g' -e 's/^Scr/ScrMem::/g'
