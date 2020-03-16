importpath "."
importpath ".."
importpath "../ff"
var $x i32
importpath "/a/bb/ccc/"
func $foo (var %y i32) void {
  dassign $x (dread i32 %y)
}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
