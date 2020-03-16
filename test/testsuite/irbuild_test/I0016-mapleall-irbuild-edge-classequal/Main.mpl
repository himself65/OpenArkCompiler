type $RR <class {@e1 i32, @e2 f32, @e3 f64}>
type $SS <class <$RR> {@f1 i32, @f2 i8, @f3 i8}>
type $TT <class {@e1 i32, @e2 f32, @e3 f64}>
type $UU <class <$RR> {@f1 i32, @f2 i8, @f3 i8}>
func &foo ( var %x <$SS>) void {
  var %y <$UU>
  dassign %y 0 ( dread agg %x 0 )
  return () }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
