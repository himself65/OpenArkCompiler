 func $foo (
  var %i i32 
#var %i1 i32, var %j1 i32, var %k1 i32
  ) i32 { 
   return (
     lnot i32(dread i32 %i))}

 func $foo1 (
  var %i i32, var %j i32, var %k i32, 
  var %i1 i32, var %j1 i32, var %k1 i32
  ) i32 { 
   return (
     lnot i32(dread i32 %i))}

 func $foo2 (
  var %i i32, var %j i32, var %k i32
  ) i32 { 
   return (
     lnot i32(constval i32 0x111111111))}

 func $foo3 (
  var %i i64, var %j i64, var %k i32
  ) i64 { 
   return (
     lnot i64(dread i64 %i))}


 func $foo5 (
  var %i i64, var %j i64, var %k i32
  ) i64 { 
   return (
     lnot i64(constval i64 0x11111))}

# todo float lnot
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
