# punch_it
Punch_It is simple console application to convert NASTRAN punch files to csv files.

Simply type punch_it followed by any NASTRAN punch files you wish to convert.

By default the case 'Subtitle' and 'Subcase' will be output per entry line in the csv output.

Use the following modifiers to control the output:

-t ----> Output the case 'Title' per line entry
-s ----> Output the case 'Subtitle' per line entry
-l ----> Output the case 'Label' per line entry

The Subcase is always output.
