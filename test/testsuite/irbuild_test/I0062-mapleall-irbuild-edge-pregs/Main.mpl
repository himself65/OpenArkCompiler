type $atyp <[10] f32>
type $aaaa <f32>
var $fff <$atyp>
func $foo ( var %i i32 )i32 

func $fact (
 var %n i32, var %m f32) i32 {
 regassign i32 %987 (constval i32 '0')
 if (ne i32 i32 (regread i32 %987, constval i32 10)) {
    eval (regread i32 %987)
    call &foo(
      sub i32 (regread i32 %1, constval i32 1))
    return (regread i32 %%retval)}
   else {
     regassign i32 %987 (constval i32 111)
     return (constval i32 '1') } 
 }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
