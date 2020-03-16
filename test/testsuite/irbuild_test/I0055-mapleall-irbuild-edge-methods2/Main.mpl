type $RR <class {
  @e1 i32, 
  @e2 f32, 
  @classname "foo",
  @e3 f64}>
type $SS <class <$RR> { @accessflag 1234, 
                        &method1(agg)agg,
                        &method2(void)void}>
func $foo (
	var %x <$SS>) i32 {
  dassign %x 2 ( constval i32 32 )
  return ( dread i32 %x 2 ) }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
