var $g i32
var $h i32
func $multiwayfunc ( var %n i32) i32 {
 multiway (dread i32 %n) @labdft {
   (add i32 (dread i32 $g, dread i32 $h)): goto @lab0
   (dread i32 $h): goto @lab1
   (constval i32 888): goto @lab0
   (constval i32 'Y'): goto @lab0
   (neg i32 (dread i32 $h)) : goto @lab9 }
@lab0
 return (constval i32 -3)
@labdft
 return (constval i32 100)
@lab9
 return (constval i32 9)
@lab1
 return (constval i32 1) }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
