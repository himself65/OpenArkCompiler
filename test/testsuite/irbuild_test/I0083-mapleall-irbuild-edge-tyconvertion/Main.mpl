 # a function returning unary(i + j) * -998
#type $SS <struct {@f1 f32}>
 func $foo (
  var %k <struct {@f1 f32}>,
  var %f f32,
  var %i i32,
   var %j i32)i64  { 
   return (
       cvt i64 i32 (constval i32 12345))}

 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
