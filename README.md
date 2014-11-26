cvtmount
========

centeroid to gcode

cvtmount.c is from the yahoo group for csm84 pnp machine.

Example usage:

cvtmount.exe feeder.txt Pick_Place_for_SE_SG_IF_V2.txt  SE_SG_IF_V2.top SE_SG_IF_V2.bot

Edit mounting file, add board orign and optional

O0  Board_Zero  95  92 -22     110011

On bot file add board X offset too , x offset is the board X lenght

O0  Board_Zero  95  -92 -22     110011
O1  Board_Offset 300.2  0 0     110011


awk.exe -f mount.awk  SE_SG_IF_V2.top >  SE_SG_IF_V2.nct
awk.exe -f mount.awk  SE_SG_IF_V2.bot >  SE_SG_IF_V2.ncb

nct and ncb are gcode to be used with mach3/linuxCNC/reprap

origins-v6.ulp is for eagle v6 for getting centroids file.
origins.awk is for converting the origins.ulp output to the format needed by cvtmount.exe
example:
awk.exe -f origins.awk test.origins.csv > test.csv

feeder.awk is for getting the not configured feeders.

Example:
echo "#" >> test.fdr
cvtmount.exe test.fdr test.csv test.top  | awk.exe -f feeder.awk >> test.fdr

Now edit test.fdr to meet you needs.
The example output is this:
99 M1206-61-23RGBC/TR8 0 1
99 PLCC6-61-23RGBC/TR8 0 1

The feeder format is:
#NR ID ROTation Head Z-height
99 M1206-61-23RGBC/TR8 0 1

nr is from 0 to 99
Head is the head or nozzle to use
optional Height is for component height, if not specified, the default is used.

Mounting format:
O0  Board_Zero  95  -92 -22     110011
O1  Board_Offset 300.2  0 0     110011
M0  LED1  17.70  8.90  -90.00  100001
M1  LED2  27.40  8.90  -90.00  100001
M2  LED3  37.00  8.90  -90.00  100001

O0 = Offset for Board Zero
O1 = Board X lenght
MNNN = Mount lines with ref,x,y,rot and flags

Flags are: HZZZFF
where H=Head/Nozzle , Z=component height in 2.1 format, FF=feeder id.

