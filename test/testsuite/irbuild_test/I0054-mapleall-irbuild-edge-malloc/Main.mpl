func &foo ( var %i i32 ) void {
   var %p ptr
   dassign %p ( malloc ptr ( constval i32 8 ))
   dassign %p ( alloca ptr ( constval i32 16 ))
   dassign %p ( gcmalloc ref i32 )
   dassign %p ( gcmallocjarray ref <[] i32> ( constval i32 5 ))
   free ( dread ref %p)
}

 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
