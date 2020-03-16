 flavor 1
 func &ciori32 (
  var %i i32, var %j i32, var %k i32
  ) i32 { 
   if (cior i32 ( cior i32(dread i32 %i, dread i32 %j), dread i32 %k)) {
     return (constval i32 1)
   }
   return (
     cior i32(dread i32 %i, dread i32 %j))}

 func &candi64 (
  var %i i64, var %j i64, var %k i64
  ) i32 { 
   if (cand i32 (cand i32(dread i32 %i, dread i32 %j), dread i32 %k)) {
     return (constval i32 1)
   }
   return (
     cand i64(dread i64 %i, dread i64 %j))}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
