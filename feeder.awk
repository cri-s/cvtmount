#Failed to match part 'PLCC6' to any component.
/Failed to match part/ { part[substr($5,2,length($5)-2)]++; }

END { for(i in part) print 99,i,0.0,1; }
