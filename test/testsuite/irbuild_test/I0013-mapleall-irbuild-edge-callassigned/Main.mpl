func $singlefoo(var %a i32) i32 {
  return (dread i32 %a)
}

func $fact(var %n i32) i32 {
  var %m i32
  var %r1 i32
  var  %r2 i32
  var  %r3 i32
  callassigned &singlefoo(dread i32 %n) {
    dassign %m 0
  }
  # Not correct, but for parse and dump test.
  callassigned &singlefoo(dread i32 %n) {
    dassign %r1 1
    dassign %r2 2
    dassign %r3 3
  }
  return (dread i32 %n)
}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
