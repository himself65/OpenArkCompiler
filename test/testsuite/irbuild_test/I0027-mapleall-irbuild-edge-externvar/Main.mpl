var $extern_var extern i32 public

func $test_extern_var ( var %i i32 ) i32 {
  return (dread i32 $extern_var) }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
