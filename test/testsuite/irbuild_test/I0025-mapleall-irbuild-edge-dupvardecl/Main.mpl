var $g <*ptr>
var $h <*ptr> = addrof ptr $g
var $g <*ptr> = addrof ptr $h
var $h <*ptr>
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
