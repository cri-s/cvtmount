#M2  X103  -1673.23  3740.16  0.00  300062
BEGIN {
	default_z=1;	# default component height
	pick_z =  6.5;	# height for pick
	place_z=  9;	# height for place
	pick_dly =  0.2;
	place_dly=  0.25;
	feedrate = 10800;
	feedrateR = 10800;
	offset_x  = 0;
	offset_y  = 0;
	offset_z  = 0;	
	G53	  = "G53";
	G53	  = "";
      	feeder_z = -22  # absolute height for Feeder move
    	park = "C0Z0X540Y-390"
	mkFeeder(1,22,3,-50,22,0,0); # mk feeder 1-22
	// changed g53g to g21g
	
	
	init();

}

END {
	print G53 "G0Z-22" # safe Z
	print G53 "G0" park
}
function mkFeeder(start,count,x,y,xx,yy,rot,i) 
{
# if count is negativ, it is a strip feeder
# if count is positiv, it is a reel/tube feeder
	for(i=start;i<(start+count);i++) {
		feeder_x[i]=x; x+=xx;
		feeder_y[i]=y; y+=yy;
		feeder_cnt[i]=0;
		feeder_xx[i]=0;
		feeder_yy[i]=0;
		feeder_rot[i]=rot;
	}
	if(count<0) {
		feeder_x[i]=x; 
		feeder_y[i]=y;
		feeder_cnt[i]=-count;
		feeder_xx[i]=xx;
		feeder_yy[i]=yy;
		feeder_rot[i]=rot;
	}
}

#M2  X103  -1673.23  3740.16  0.00  300062
NF == 6 && length($6==6) && 0+substr($6,1,1)!=0 {
	process(substr($1,1,1),substr($1,2),$2,$3,$4,$5,
		substr($6,1,1),substr($6,2,3),substr($6,5,2));
	next;
}

NF { print "Error on line " FNR ": " $0; }

function process(type,nr,ref,x,y,r,n,z,f) {
	f=0+f; x=0+x; y=0+y; r=0+r; n=0+n; z=0+z; f=0+f; 
    if (type=="M") { 
    	z/=10; if(z==0.0) z=default_z;
	print "N" nr*100+1 ,"( " type nr " " ref " )"
	feeder(f);
	if(f>=1&&f<=32)
	  print G53 "G1Y" (feeder_y[f]+48)  "F" feedrate 
	pick(z)
	rot(r)
	if(f>=1&&f<=32)
	  print G53 "G1Y" (feeder_y[f])  "F" feedrate 
	move(x,y);
	place(z);
   }
    if (type=="O") {
	if(nr!="0"&&nr!="1") { print "Error:",$0; exit(-1); }
	  offsets[nr,"X"]=x;
	  offsets[nr,"Y"]=y;
	  offsets[nr,"Z"]=r;
	  offset_x=offsets["0","X"]+offsets["1","X"];
	  offset_y=offsets["0","Y"]+offsets["1","Y"];
	  offset_z=offsets["0","Z"]+offsets["1","Z"];
	print	"( offsets: " xyz(offset_x,offset_y,offset_z) " )"
   }
}
function move(x,y) {  y=0+y; # y=y*-1; # HACK for nenad
  if(offset_z) 
	print "G1X" x+offset_x "Y" y+offset_y  "Z" offset_z "F" feedrate  " (xy)" ;
  else
	print "G1X" x+offset_x "Y" y+offset_y  "F" feedrate  " (xy)" ;
}
function rot(r) {
	while(r> 180) r-=360;
	while(r<-180) r+=360;
	if(r==0.0) return;
	print "G1C" r+offset_c "F" feedrateR  " (rot)"

}
function place(z) {
	print "G1Z" (-place_z+z+offset_z) "F" feedrate "( place " z " )"	# move down
	print "M5"		# vacuum off
	print "G4P" place_dly	# wait
	if(place_z-z-0.7>place_z/2)
		print "G0Z" (-place_z/2+offset_z) # move up
	else
		print "G0Z"  offset_z  # move up
}
function pick(z) {
	print "G91 (pick " z ")"# relative coordinate
	print "G0Z" (-pick_z+2+z)     # move down
	print "M4"			# vacuum on
	print "G1Z-2"  "F" feedrate   # move down
	print "G4P" pick_dly	# wait
	print "G1Z" (pick_z-z)  "F" feedrate
	print "G90"		# absolute coordinate
}
function	feeder(f,x,y) { f=0+f;
	if(!feeder_x[f]) { print "Error, unknown feeder " f " on line " FNR ": " $0 ; return; }
	x=feeder_x[f]; y=feeder_y[f];
	feeder_x[f]+=feeder_xx[f]; 		
	feeder_y[f]+=feeder_yy[f]; 		
	if(--feeder_cnt[f]==0) {
		feeder_xx[f]=0; 		
		feeder_yy[f]=0; 		
	}
	r+=feeder_rot[f];
	print G53 "G0X" x "Y" y "Z" feeder_z "C0 (feeder " f ")"
}
function xy(a,b) { return sprintf("X%5.3fY%5.3f",a,b); }
function xyz(a,b,c) { return sprintf("X%5.3fY%5.3fZ%5.3f",a,b,c); }
function x(v) { return sprintf("X%5.3f",v); }
function y(v) { return sprintf("Y%5.3f",v); }
function z(v) { return sprintf("Z%5.3f",v); }
function c(v) { return sprintf("C%5.3f",v); }
function f(v) { return sprintf("F%8.3f",v); }

function init() {
	print	"(init)"
	print	"g21 (mm)"
	print	"g17 (xy plane)"
	print	"g90 (absolute)"
#	print	"g91.1 (arc centers are relative to arc starting point)"
	print	"g40 (disable cutter comp)"
	print	"g54 (switch to coordinate system 1)"
	print	"g94 (units/minute feed mode)"
#	print	"g99 (in canned cycles, retract to the Z coordinate specified by the R word)"
	print	"g64 p0.0005 (enable path blending, but stay withing 0.0005 of the programmed path)"
	print	"g49 (turn off tool length compensation)"
	print	"g80 (turn off canned cycles)"
	print	"(init done)"
}
