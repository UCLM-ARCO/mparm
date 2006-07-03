#!/bin/sh

grep "Processor" | gawk --non-decimal-data '
{
  if ($3 == "shuts") {
  }
  else if ($4 == "Test") {
    if (!pass[$5]) {
      print "Size\t" $5
      pass[$5] = 1
    }
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
}' | gawk --non-decimal-data '
{
  if ($1 == "Size") {
    title = $2
    titles[$2] = $2
    if (!main) main = $2
  }
  else {
    val[title,$1] = $2
    if (title == main) sizes[$1] = $1
  }
}
END {
  n = asort(titles)
  m = asort(sizes)

  printf "Size"
  for (j=1; j<=n; ++j)
    printf "\t" titles[j]
  printf "\n"

  for (i=1; i<=m; ++i) {
    printf sizes[i]
    for (j=1; j<=n; ++j) {
      printf "\t" val[titles[j],sizes[i]]
    }
    printf "\n"
  }
}'
