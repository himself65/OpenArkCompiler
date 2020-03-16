 func $cvtu64tof64 (
  var %i u64
  ) f64 { 
   return (
     cvt f64 u64(dread u64 %i))}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
