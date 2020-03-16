 func $ceilf64toi32 (
  var %i f64
  ) i32 { 
   return (
     ceil i32 f64(dread f64 %i))}

 func $ceilf64toi64 (
  var %i f64
  ) i64 { 
   return (
     ceil i64 f64(dread f64 %i))}

 func $ceilf32toi32 (
  var %i f32
  ) i32 { 
   return (
     ceil i32 f32(dread f32 %i))}


 func $ceilf32toi64 (
  var %i f32
  ) i64 { 
   return (
     ceil i64 f32(dread f32 %i))}

# todo float ceil
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
