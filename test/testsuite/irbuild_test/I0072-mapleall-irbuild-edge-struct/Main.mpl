# typedef struct ss {
#   int f1;
#   char f2:3;
#   char f3:5;
# } SS;
# SS foo(SS x) {
#   x.f2 = 32;
#   return x;
# }
type $SS <struct {@f1 i32, @f2:3 i8, @f3:5 i8}>
func $foo (
	var %x <$SS>) i32 {
  dassign %x 2 ( constval i32 32 )
  return ( dread i32 %x 2 ) }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
