func $fact (
 var %n i32) i32 {
 var %foo <func(i32) i32>
 var %r1 i32
 var %r2 i32
 var %r3 i32
 if (ne i32 i32 (dread i32 %n, constval i32 1)) {
    icallassigned ( dread a64 %foo,
      sub i32 (dread i32 %n, constval i32 1)) {
      dassign %r1 0
    }
    # Not correct, but for parse and dump test.
    icallassigned ( dread a64 %foo,
      sub i32 (dread i32 %n, constval i32 1)) {
      dassign %r1 1
      dassign %r2 2
      dassign %r3 3
    }
    return (dread i32 %n)
  }
}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
