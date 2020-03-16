# float a[10];
# void init(void) {
#   int i;
#   for (i=0; i<10; i++)
#     a[i] = i*3;
# }

var $a <[10] f32>
var $f1 f32
func $init () void {
  var %i i32
  dassign %i ( constval i32 0 )
  while (
    lt i32 i32 ( dread i32 %i, constval i32 10) ) {
      iassign <* [10] f32> (
        array 1 a32 <* [10] f32> (addrof a32 $a, dread i32 %i),
	      mul f32 (dread f32 $f1, constval f32 3.33f) ) 
      dassign %i (
        add i32 (dread i32 %i, constval i32 1) )} }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
