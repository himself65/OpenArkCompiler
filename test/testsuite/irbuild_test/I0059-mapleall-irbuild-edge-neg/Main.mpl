 func $foo (
  var %i i32 
#var %i1 i32, var %j1 i32, var %k1 i32
  ) i32 { 
   return (
     neg i32(dread i32 %i))}

 func $foo1 (
  var %i i32, var %j i32, var %k i32, 
  var %i1 i32, var %j1 i32, var %k1 i32
  ) i32 { 
   return (
     neg i32(dread i32 %i))}

 func $foo2 (
  var %i i32, var %j i32, var %k i32
  ) i32 { 
   return (
     neg i32(constval i32 0x111111111))}

 func $foo3 (
  var %i i64, var %j i64, var %k i32
  ) i64 { 
   return (
     neg i64(dread i64 %i))}


 func $foo5 (
  var %i i64, var %j i64, var %k i32
  ) i64 { 
   return (
     neg i64(constval i64 0x11111))}

 func $foo6 (
  var %i f64
  ) f64 { 
   return (
     neg f64(dread f64 %i))}

 func $foo7 (
  var %i f32
  ) f32 { 
   return (
     neg f32(dread f32 %i))}

 func $foo8 (
  var %i f64
  ) f64 { 
   return (
     neg f64(constval f64 -1.24))}

 func $foo9 (
  var %i f32
  ) f32 { 
   return (
     neg f32(constval f32 -1.24f))}
# todo float neg
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
