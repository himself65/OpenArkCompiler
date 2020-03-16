type $RR <class {@e1 i32, ^e2 f32, @e3 f64}>
type $SS <class <$RR> {^f1 i32, @f2 i8, ^f3 i8}>
javaclass $TT <$SS> public static
func &foo (
	var %x <$SS>) i32 {
  dassign %x -1 ( constval i32 32 )
  return ( dread i32 %x -2 ) }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
