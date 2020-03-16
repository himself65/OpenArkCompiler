var $g ptr
var $h i32
func $multiwayfunc ( var %p ptr) ptr {
 multiway (dread ptr %p) @labdft {
   (dread ptr $g): goto @lab1
   (add ptr (dread i32 $g, dread i32 $h)): goto @lab0
   (conststr ptr "world"): goto @lab9 }
@lab0
 return (dread ptr $g)
@labdft
 return (conststr ptr "hello")
@lab9
 return (add ptr (conststr ptr "foo", dread i32 $h))
@lab1
 return (addrof ptr $h) }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
