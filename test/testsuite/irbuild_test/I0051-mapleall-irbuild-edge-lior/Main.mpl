 func $liori32 (
  var %i i32, var %j i32
  ) i32 { 
   return (
     lior i32(dread i32 %i, dread i32 %j))}

 func $liori64 (
  var %i i64, var %j i64
  ) i32 { 
   return (
     lior i64(dread i64 %i, dread i64 %j))}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
