type $__field_meta__ <struct {
  @fieldname i32,
  @offset i32,
  @declaringclass <* void>,
  @type <* void>}>
var $v1 <* void> public
var $v2 <* void> public
var $vs <[1] <$__field_meta__>> public = [[1= 0x35ad1b, 2= 0xabcd, 3= addrof ptr $v2, 4= addrof ptr $v1]]
var $vvs <[2][1] <$__field_meta__>> public = [[[1= 11, 2= 22, 3= addrof ptr $v1, 4= addrof ptr $v1]],
                                              [[1= 33, 2= 44, 3= addrof ptr $v2, 4= addrof ptr $v2]]]
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
