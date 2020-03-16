func $fact (
 var %n i32) i32 {
@fred2
 if (ne i32 i32 (dread i32 %n, constval i32 1)) {
     brfalse @fred2 (dread i32 %n)
@fred1
     return (constval i32 0) } 
 else {
     return (constval i32 1) } }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
