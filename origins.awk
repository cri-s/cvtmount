BEGIN {FS="\t"; OFS="	";
#top,"LED2",1077.2,350.0,270.0,"PLCC6",100n
#     1          2             3            4              5           6
print "Designator Footprint          Mid X         Mid Y         Ref X         Ref Y         Pad X         Pad Y TB      Rotation Comment"
getline ; t=$1;  getline
print t,$0
}
NF { 
	gsub(/"/,"");
 #remove quotes
print $2,$6 , $3 "mm",$4  "mm", $3 "mm",$4  "mm", $3 "mm",$4  "mm", $1 , $5 , $7
}
