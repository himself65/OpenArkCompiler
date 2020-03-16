type $I1 <interface {@g0 i32}>
type $I2 <interfaceincomplete {@g1 i32}>
type $RR <interface <$I1> <$I2> {@e1 i32, &method1(agg)agg, &method2(void)void}>
javainterface $UU <$RR> public static
type $CC <interface {@h0 i32}>
type $TT <class <$CC> {@f1 i32, @f2 i8, @f3 i8, $I1, $I2, $RR}>
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
