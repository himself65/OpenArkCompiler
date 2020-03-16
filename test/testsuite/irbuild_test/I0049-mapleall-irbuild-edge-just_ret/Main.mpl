type $atyp <[10] f32>
var $fff <i32> = 7789
func $foo ( var %i i32 )i32 

func $fact (
 var %n i32, var %m f32) void {
  return () }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
