 func $foo (
  var %i i32 
#var %i1 i32, var %j1 i32, var %k1 i32
  ) i32 { 
   return (
     depositbits i32 1 23(
       constval i32 0xfff, constval i32 0x11))}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
