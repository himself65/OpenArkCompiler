 func $cvtu64tof32 (
  var %i u64
  ) f32 { 
   return (
     cvt f32 u64(dread u64 %i))}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
