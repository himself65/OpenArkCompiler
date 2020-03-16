 func $foo (
  var %i i32 
#var %i1 i32, var %j1 i32, var %k1 i32
  ) i32 { 
   return (
     add i32(dread i32 %i, 
       constval i32 -998))}

 func $foo1 (
  var %i i32, var %j i32, var %k i32, 
  var %i1 i32, var %j1 i32, var %k1 i32
  ) i32 { 
   return (
     add i32(dread i32 %i, dread i32 %j))}

 func $foo2 (
  var %i i32, var %j i32, var %k i32
  ) i32 { 
   return (
     add i32(dread i32 %i, constval i32 0x111111111))}

 func $foo3 (
  var %i i64, var %j i64, var %k i32
  ) i64 { 
   return (
     add i64(dread i64 %i, constval i64 0x111111111))}

 func $foo4 (
  var %i i64, var %j i64, var %k i32
  )i64  { 
   return (
     add i64(dread i64 %i, dread i64 %j))}

 func $foo5 (
  var %i i64, var %j i64, var %k i32
  ) i64 { 
   return (
     add i64(dread i64 %i, constval i64 0x11111))}

 func $foo6 (
  var %i f64, var %j f64, var %k i32
  ) f64 { 
   return (
     add f64(dread f64 %i, constval f64 2.237))}

 func $foo66 (
  var %i f64, var %j f64, var %k i32
  ) f64 {
   return (
     add f64(dread f64 %i, constval f64 2.237))}
 func $foo7 (
  var %i f32, var %j f32, var %k i32
  ) f32 { 
   return (
     add f32(dread f32 %i, dread f32 %j))}

 func $foo8 (
  var %i f32, var %j f32, var %k i32
  ) f32 { 
   return (
     add f32(dread f32 %i, constval f32 2.237f))}

 func $foo9 (
  var %i i8, var %j u8, var %k i32) i32 { 
   return (
     add i32(dread i32 %i, dread u32 %j))}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
