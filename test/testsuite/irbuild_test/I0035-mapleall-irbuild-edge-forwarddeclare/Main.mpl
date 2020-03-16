var $test_var i32 public

func $test_extern_var ( var %i i32 ) i32 {
  return (dread i32 $test_var) 
}

var $test_var i32 public = 0
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
