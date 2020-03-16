type $RR <class {@e1 i32, @e2 f32, @e3 f64}>
type $SS <class <$RR> { &method1(agg)agg,
                        &method2(void)void}>
func $foo (
  var %x <$SS>) i32 {
  var %r1 i32
  var  %r2 i32
  var  %r3 i32
  dassign %x 2 ( constval i32 32 )
  # Not correct, but for parse and dump test.
  virtualcallassigned &method2(addrof ptr %x) {
    dassign %r1 1
    dassign %r2 2
    dassign %r3 3
  }
  superclasscallassigned &method2(addrof ptr %x) {
    dassign %r1 1
    dassign %r2 2
    dassign %r3 3
  }
  interfacecallassigned &method2(addrof ptr %x) {
    dassign %r1 1
    dassign %r2 2
    dassign %r3 3
  }
  return ( dread i32 %x 2 ) }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
