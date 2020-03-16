 func $fooi64 (
  var %i i64, var %j i64
  ) i64 { 
   return (
     add i64(dread i64 %i, dread i64 %j))}

 func $addi64I (
  var %i i64
  ) i64 { 
   return (
#add i64(dread i64 %i, constval i64 0x123456789))}
     add i64(dread i64 %i, constval i64 0x800000115))}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
