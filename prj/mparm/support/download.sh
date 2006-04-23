#!/bin/sh


mkdir -p orig

cat <<EOF

Sorry, but SystemC files are not easy to download from a
script. You must manually point to:

  https://www.systemc.org/projects/systemc/files

And log in with your own using your private Login and Password
combination in the upper right corner of the page.

Source files must be stored in orig/ subdirectory. The following
files will be automatically packaged:

- systemc-2.1.v1.tgz
- systemc-2.0.1.tgz

EOF
