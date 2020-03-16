 func &addf32r(
  var %i f32, var %j f32
  ) f32 { 
   return (
     add f32(dread f32 %i, dread f32 %j))}

 func &addf32I (
  var %i f32
  ) f32 { 
   return (
     add f32(dread f32 %i,
       constval f32 1.234f))}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
