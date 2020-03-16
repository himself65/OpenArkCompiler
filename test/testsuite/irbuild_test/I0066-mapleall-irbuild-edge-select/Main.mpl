 func $selecti32 (
  var %i i32,
   var %j i32,
   var %k i32) i32 { 
   return (
     select i32 (
      dread i32 %i, 
      dread i32 %j, 
      dread i32 %k))}

 func $selecti64 (
  var %i i32,
   var %j i64,
   var %k i64) i64 { 
   return (
     select i64 (
      dread i32 %i, 
      dread i64 %j, 
      dread i64 %k))}

 func $selectf32 (
  var %i i32,
   var %j f32,
   var %k f32) f32 { 
   return (
     select f32 (
      dread i32 %i, 
      dread f32 %j, 
      dread f32 %k))}

 func $selectf64 (
  var %i i32,
   var %j f64,
   var %k f64) f64 { 
   return (
     select f64 (
      dread i32 %i, 
      dread f64 %j, 
      dread f64 %k))}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
