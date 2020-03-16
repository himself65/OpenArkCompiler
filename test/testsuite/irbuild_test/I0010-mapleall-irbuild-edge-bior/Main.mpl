 func $bior32I (
  var %i i32
  ) i32 { 
   return (
     bior i32(dread i32 %i,
       constval i32 0x12))}	  
 func $bior32I_2 (
  var %i i32
  ) i32 { 
   return (
     bior i32(dread i32 %i,
       constval i32 0x112))}	 
 func $bior32RR (
  var %i i32, var %j i32
  ) i32 { 
   return (
     bior i32(dread i32 %i,
       dread i32 %j))}	   

 func $bior64I (
  var %i i64
  ) i64 {
   return (
     bior i64(dread i64 %i,
       constval i64 0x1200000015))}
 func $bior64I_2 (
  var %i i64
  ) i64 {
   return (
     bior i64(dread i64 %i,
       constval i64 0x1200000115))}
 func $bior64RR (
  var %i i64,
  var %j i64
  ) i64 {
   return (
     bior i64(dread i64 %i,
              dread i64 %j))}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
