# int foo(void) {
#   int n = 1;
#   int i;
#   for (i=1; i<10; i++)
#     n = n * i;
# }

func $foo (var %i i32) i32 {
  while (
    lt i32 i32 ( dread i32 %i, constval i32 10) ) {
      dassign %i (
	mul i32 (dread i32 %i, dread i32 %i))
      dassign %i (
        add i32 (dread i32 %i, constval i32 1) )} 
  return ( constval i32 1 )
}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
