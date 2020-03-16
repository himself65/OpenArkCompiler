 func $retypef64toi64 (
  var %i f64
  ) i64 { 
   return (
     retype i64 <i64> (dread f64 %i))}

 func $retypei64tof64 (
  var %i i64
  ) f64 { 
   return (
     retype f64 <f64> (dread i64 %i))}

 func $retypef32toi32 (
  var %i f32
  ) i32 { 
   return (
     retype i32 <i32> (dread f32 %i))}

 func $retypei32tof32 (
  var %i i32
  ) f32 { 
   return (
     retype f32 <f32> (dread i32 %i))}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
