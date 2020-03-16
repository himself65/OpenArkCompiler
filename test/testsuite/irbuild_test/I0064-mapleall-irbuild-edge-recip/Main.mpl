 func $foo6 (
  var %i f64
  ) f64 { 
   return (
     recip f64(dread f64 %i))}

 func $foo7 (
  var %i f32
  ) f32 { 
   return (
     recip f32(dread f32 %i))}

 func $foo8 (
  var %i f64
  ) f64 { 
   return (
     recip f64(constval f64 -1.24))}

 func $foo9 (
  var %i f32
  ) f32 { 
   return (
     recip f32(constval f32 -1.24f))}
# todo float recip
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
