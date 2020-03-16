 func $floorf64toi32 (
  var %i f64
  ) i32 { 
   return (
     floor i32 f64(dread f64 %i))}

 func $floorf64toi64 (
  var %i f64
  ) i64 { 
   return (
     floor i64 f64(dread f64 %i))}

 func $floorf32toi32 (
  var %i f32
  ) i32 { 
   return (
     floor i32 f32(dread f32 %i))}


 func $floorf32toi64 (
  var %i f32
  ) i64 { 
   return (
     floor i64 f32(dread f32 %i))}

# todo float floor
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
