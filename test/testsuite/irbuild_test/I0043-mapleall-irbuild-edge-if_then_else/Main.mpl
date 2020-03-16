type $atyp <[10] f32>
var $fff <i32> = 7789
func $foo ( var %i i32 )i32 

func $fact (
 var %n i32, var %m f32) i32 {
 type %i32ty <struct {
        @FF1 i32,
        @FF2 f64 }>
 var %alocal <%i32ty>
 dassign %alocal (constval i32 999)
 if (ne i32 i32 (dread i32 %n, constval i32 1)) {
    eval (dread i32 %n)
    call &foo(
      add i32 (dread i32 %n, constval i32 1))
    return (regread i32 %%retval)}
   else {
     return (constval i32 1) } }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
