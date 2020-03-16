 func $foo1 (
  var %i i32, var %j i32
  ) i32 { 
   return (
     add i32(dread i32 %i, dread i32 %j))}

 func $addi32I (
  var %i i32
  ) i32 { 
   return (
     add i32(dread i32 %i,
#constval i32 0xfff))}
#       constval i32 0xffff))}
       constval i32 0xfffff))}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
