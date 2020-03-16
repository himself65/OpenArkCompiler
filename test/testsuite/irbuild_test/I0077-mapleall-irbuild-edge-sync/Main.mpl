#     1  public class sync {
#     2      private Object go = new Object();
#     3      private int waiters = 0;
#     4
#     5      public void foo() {
#     6          synchronized (go) {
#     7              waiters++;
#     8          }
#     9      }
#    10  }

# some unrelated functions are removed
type $Ljava/lang/Object; <class  {}>
type $Lsync; <class <$Ljava/lang/Object;> {
  @go <* <$Ljava/lang/Object;>> private,
  @waiters i32 private,
  &Lsync;|foo|__V|(<* <$Lsync;>>) void}>
func &Lsync;|foo|__V| (var %_this <* <$Lsync;>>) void {
  var %Reg1_R33 <* <$Ljava/lang/Object;>>
  var %Reg0_I i32
  var %Reg0_R38 <* void>
  #LINE sync.java:6 
  dassign %Reg1_R33 0 (iread ptr <* <$Lsync;>> 1 (dread ptr %_this 0))
  syncenter (dread ptr %Reg1_R33 0)
  #LINE sync.java:7 
  dassign %Reg0_I 0 (iread i32 <* <$Lsync;>> 2 (dread ptr %_this 0))
  dassign %Reg0_I 0 (add i32 (
      dread i32 %Reg0_I 0,
      cvt i32 i8 (constval i8 1)))
  iassign <* <$Lsync;>> 2 (dread ptr %_this 0, dread i32 %Reg0_I 0)
  #LINE sync.java:8 
  syncexit (dread ptr %Reg1_R33 0)
  #LINE sync.java:9 
  return ()
  #LINE sync.java:8 
  dassign %Reg0_R38 0 (regread ptr %%thrownval)
  syncexit (dread ptr %Reg1_R33 0)
  throw (dread ptr %Reg0_R38 0)
}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
