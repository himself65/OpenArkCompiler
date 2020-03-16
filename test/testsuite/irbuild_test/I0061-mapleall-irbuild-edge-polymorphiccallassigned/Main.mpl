type $SS <class {@f1 i32, @f2 i8, @f3 i8, &fun(i32) i32}>
javaclass $TT <$SS> public static

func &bar (var %y i32) i32 {
  return (dread i32 %y)
}

func &foo (var %x <$SS>) i32 {
  var %ret i32
  polymorphiccallassigned <func(i32) i32> &bar (dread i32 %x 1) {dassign %ret 0}
  return (dread i32 %ret)
}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
