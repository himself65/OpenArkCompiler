# typedef struct { int f1, char f2, unsigned short f3[]} SS;
# void foo(SS *x, SS *y) {
#   int i, j;
#   x->f3[ij] = y->f3[j];
# }

type $SS <struct {@f1 i32, @f2 i8, @f3 <[] u16>}>
func $foo ( var %x <*<$SS>>, var %y <*<$SS>>) void {
  var %i i32
  var %j i32
  iassign <* u16> ( array 1 ptr <*[]u16> (
                iaddrof ptr <* <$SS>> 3 (dread ptr %x), dread i32 %i),
                iread u16 <* u16> ( array 1 ptr <*[]u16> (
                    iaddrof ptr <* <$SS>> 3 (dread ptr %y), dread i32 %j)))
  return()  }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
