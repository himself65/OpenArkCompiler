 func $foo6 (
  var %i f64
  ) f64 {
   return (
     sqrt f64(dread f64 %i))}

 func $foo7 (
  var %i f32
  ) f32 {
   return (
     sqrt f32(dread f32 %i))}

 func $foo8 (
  var %i f64
  ) f64 {
   return (
     sqrt f64(constval f64 4.0))}

 func $foo9 (
  var %i f32
  ) f32 {
   return (
     sqrt f32(constval f32 4.0f))}
# todo float sqrt
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
