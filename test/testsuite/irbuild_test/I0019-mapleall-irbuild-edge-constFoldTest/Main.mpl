# 1 + 8 -> 9
func $testSum (var %i i32) i32 {
  return (
    add i32(constval i32 1, constval i32 8))
}

# sqrt(4.0) -> 2.0
func $testSqrt (var %i i32) f32 {
  return (
    sqrt f32(constval f32 4.0f))
}

# 5 + (5 + a) -> a + 10
func &testConstVarFold00 (var %a i32) i32 {
  return (add i32 (
    constval i32 5,
    add i32 (constval i32 5, dread i32 %a)))
}

# 5 + (5 - a) -> 10 - a
func &testConstVarFold01 (var %a i32) i32 {
  return (add i32 (
    constval i32 5,
    sub i32 (constval i32 5, dread i32 %a)))
}

# 5 + (a + 5) ->  a + 10
func &testConstVarFold02 (var %a i32) i32 {
  return (add i32 (
    constval i32 5,
    add i32 (dread i32 %a, constval i32 5)))
}

# 6 + (a - 5) ->  a + 1
func &testConstVarFold03 (var %a i32) i32 {
  return (add i32 (
    constval i32 6,
    sub i32 (dread i32 %a, constval i32 5)))
}

# 5 * (5 * a) -> 5 * (5 * a)
func &testConstVarFold04 (var %a i32) i32 {
  return (mul i32 (
    constval i32 5,
    mul i32 (constval i32 5, dread i32 %a)))
}

# 5 * (a * 5) -> 5 * (a * 5)
func &testConstVarFold05 (var %a i32) i32 {
  return (mul i32 (
    constval i32 5,
    mul i32 (dread i32 %a, constval i32 5)))
}

# 3 - (2 - a) -> a + 1
func &testConstVarFold06 (var %a i32) i32 {
  return (sub i32 (
    constval i32 3,
    sub i32 (constval i32 2, dread i32 %a)))
}

# 3 - (2 + a) -> 1 - a
func &testConstVarFold07 (var %a i32) i32 {
  return (sub i32 (
    constval i32 3,
    add i32 (constval i32 2, dread i32 %a)))
}

# 3 - (a + 2) -> 1 - a
func &testConstVarFold08 (var %a i32) i32 {
  return (sub i32 (
    constval i32 3,
    add i32 (dread i32 %a, constval i32 2)))
}

# 3 - (a - 2) -> 5 - a
func &testConstVarFold09 (var %a i32) i32 {
  return (sub i32 (
    constval i32 3,
    sub i32 (dread i32 %a, constval i32 2)))
}

# (2 + a) - 3 -> a - 1
func &testConstVarFold10 (var %a i32) i32 {
  return (sub i32 (
    add i32 (constval i32 2, dread i32 %a),
    constval i32 3))
}

# (2 - a) - 3 -> -a - 1
func &testConstVarFold11 (var %a i32) i32 {
  return (sub i32 (
    sub i32 (constval i32 2, dread i32 %a),
    constval i32 3))
}

# (a + 2) - 3 -> a - 1
func &testConstVarFold12 (var %a i32) i32 {
  return (sub i32 (
    add i32 (dread i32 %a, constval i32 2),
    constval i32 3))
}

#  (a - 2) - 3 -> a - 5
func &testConstVarFold13 (var %a i32) i32 {
  return (sub i32 (
    sub i32 (dread i32 %a, constval i32 2),
    constval i32 3))
}

# 1 + (5 + ((5 - a) - 3)) -> 8 - a
func &testConstVarFold15 (var %a i32) i32 {
  return (add i32 (
    constval i32 1,
    add i32 (
      constval i32 5,
      sub i32 (
        sub i32 (constval i32 5, dread i32 %a),
        constval i32 3))))
}

# 1 + (5 + ((5 - a) * 3)) -> ((5 - a) * 3) + 6
func &testConstVarFold16 (var %a i32) i32 {
  return (add i32 (
    constval i32 1,
    add i32 (
      constval i32 5,
      mul i32 (
        sub i32 (constval i32 5, dread i32 %a),
        constval i32 3))))
}

# (-a) + b --> (-a) + b
func &testConstantFoldAdd00 (var %a i32, var %b i32) i32 {
  return (add i32 (
    neg i32 (dread i32 %a),
    dread i32 %b))
}

#  a + (-b) --> a + (-b)
func &testConstantFoldAdd01 (var %a i32, var %b i32) i32 {
  return (add i32 (
    dread i32 %a,
    neg i32 (dread i32 %b)))
}


# 5.0f + (5 - a) -> same, no folding with floating point numbers
func &testConstVarFold20 (var %a i32) i32 {
  return (add f32 (
    constval f32 5.0f,
    sub i32 (constval i32 5, dread i32 %a)))
}


func &testConstFold0 (var %a i32, var %b i32, var %c i32) i32 {
  # ((a + 5) + ((2 + c) + (4 + 2))) -> ((a + c) + 13)
  return (add i32 (
    add i32 (dread i32 %a, constval i32 0x5),
    add i32 (
      add i32 (constval i32 0x2, dread i32 %c),
      add i32 (constval i32 0x4, constval i32 0x2))))
}

func &testConstFold1 (var %a i32, var %b i32, var %c i32) i32 {
  # ((a + 5) + (2 + c)) -> ((a + c) + 7)
  return (add i32 (
    add i32 (dread i32 %a, constval i32 0x5),
    add i32 (constval i32 0x2, dread i32 %c)))
}

func &testConstFold2 (var %a i32, var %b i32, var %c i32) i32 {
  # ((a - 5) - ((b - c) - (4 - 2))) -> ((a - (b - c)) - 3)
  return (sub i32 (
    sub i32 (dread i32 %a, constval i32 0x5),
    sub i32 (
      sub i32 (dread i32 %b, dread i32 %c),
      sub i32 (constval i32 0x4, constval i32 0x2))))
}

func &testConstFold3 (var %a i32, var %b i32, var %c i32) i32 {
  # ((a - 5) + ((b - c) - (4 - 2))) -> ((a + (b - c)) - 7)
  return (add i32 (
    sub i32 (dread i32 %a, constval i32 0x5),
    sub i32 (
      sub i32 (dread i32 %b, dread i32 %c),
      sub i32 (constval i32 0x4, constval i32 0x2))))
}

func &testConstFold4 (var %a i32, var %b i32, var %c i32) i32 {
  # ((a - 5) - ((4 - 2) - (4 - b))) -> ((a - b) - 3)
  return (sub i32 (
    sub i32 (dread i32 %a, constval i32 0x5),
    sub i32 (
      sub i32 (constval i32 0x4, constval i32 0x2),
      sub i32 (constval i32 0x4, dread i32 %b))))
}

func &testConstFold5 (var %a i32, var %b i32, var %c i32) i32 {
  # ((5 - a) - ((b - 4) - (4 - b))) -> (((-a) - (b - (-b))) + 13)
  return (sub i32 (
    sub i32 (constval i32 0x5, dread i32 %a),
    sub i32 (
      sub i32 (dread i32 %b, constval i32 0x4),
      sub i32 (constval i32 0x4, dread i32 %b))))
}

func &testConstFold6 (var %a i32, var %b i32, var %c i32) i32 {
  # ((5 - a) - ((4 - b) - (b - 4))) -> (((-a) - ((-b) - b)) - 3)
  return (sub i32 (
    sub i32 (constval i32 0x5, dread i32 %a),
    sub i32 (
      sub i32 (constval i32 0x4, dread i32 %b),
      sub i32 (dread i32 %b, constval i32 0x4))))
}

func &testConstFold7 (var %a i32, var %b i32, var %c i32) i32 {
  # ((a - 5) - ((4 + 2) - (4 - (b - c)))) -> ((a - (b - c)) - 7)
  return (sub i32 (
    sub i32 (dread i32 %a, constval i32 0x5),
    sub i32 (
      add i32 (constval i32 0x4, constval i32 0x2),
      sub i32 (
        constval i32 0x4,
        sub i32 (dread i32 %b, dread i32 %c)))))
}

func &testConstFold8 (var %a i32, var %b i32, var %c i32) i32 {
  # ((a - 5) - ((4 + 2) - (4 * (b - c)))) -> ((a - (-(4 * (b - c)))) - 11)
  return (sub i32 (
    sub i32 (dread i32 %a, constval i32 0x5),
    sub i32 (
      add i32 (constval i32 0x4, constval i32 0x2),
      mul i32 (
        constval i32 0x4,
        sub i32 (dread i32 %b, dread i32 %c)))))
}

func &testConstFold9 (var %a i32, var %b i32, var %c i32) i32 {
  # ((a - 5) - ((4 + 2) - (-(4 - (b - c))))) -> ((a - (-(b - c))) - 15)
  return (sub i32 (
    sub i32 (dread i32 %a, constval i32 0x5),
    sub i32 (
      add i32 (constval i32 0x4, constval i32 0x2),
      neg i32 (sub i32 (
        constval i32 0x4,
        sub i32 (dread i32 %b, dread i32 %c))))))
}

func &testConstFold10 (var %a i32, var %b i32, var %c i32) i32 {
  # ((4 + 2) * (-(4 - (3 - c)))) -> (6 * ((-c) - 1))
  return (mul i32 (
    add i32 (constval i32 0x4, constval i32 0x2),
    neg i32 (sub i32 (
      constval i32 0x4,
      sub i32 (constval i32 0x3, dread i32 %c)))))
}

func &testConstFold11 (var %a i32, var %b i32, var %c i32) i32 {
  # ((4 + 2) * (4 - (3 - c))) -> (6 * (c + 1))
  return (mul i32 (
    add i32 (constval i32 0x4, constval i32 0x2),
    sub i32 (
      constval i32 0x4,
      sub i32 (constval i32 0x3, dread i32 %c))))
}

func &testConstFold12 (var %a i32, var %b i32, var %c i32) i32 {
  # ((4 + 2) * (~(4 - (3 - c)))) -> (6 * (~(c + 1)))
  return (mul i32 (
    add i32 (constval i32 0x4, constval i32 0x2),
    bnot i32 (sub i32 (
      constval i32 0x4,
      sub i32 (constval i32 0x3, dread i32 %c)))))
}

func &testConstFold13 (var %a i32, var %b i32, var %c i32) i32 {
  # (5 * (((4 - 2) + (2 & 4)) - (4 - (c | 3)))) -> (5 * ((c | 3) - 2))
  return (mul i32 (
    constval i32 0x5,
    sub i32 (
      add i32 (
        sub i32 (constval i32 0x4, constval i32 0x2),
        band i32 (constval i32 0x2, constval i32 0x4)),
      sub i32 (
        constval i32 0x4,
        bior i32 (dread i32 %c, constval i32 0x3)))))
}

func &testConstFold14 (var %a i32, var %b i32, var %c i32) i32 {
  # (5 * (((4 - 2) + (2 & 4)) * (4 - (c | 3)))) -> (5 * (2 * (4 - (c | 3))))
  return (mul i32 (
    constval i32 0x5,
    mul i32 (
      add i32 (
        sub i32 (constval i32 0x4, constval i32 0x2),
        band i32 (constval i32 0x2, constval i32 0x4)),
      sub i32 (
        constval i32 0x4,
        bior i32 (dread i32 %c, constval i32 0x3)))))
}

func &testConstFold15 (var %a i32, var %b i32, var %c i32) i32 {
  # ((1 + 0) * (~(4 - (0 * c)))) -> (~4)
  return (mul i32 (
    add i32 (constval i32 0x1, constval i32 0x0),
    bnot i32 (sub i32 (
      constval i32 0x4,
      mul i32 (constval i32 0x0, dread i32 %c)))))
}

func &testConstFold16 (var %a i32, var %b i32, var %c i32) i32 {
  # ((~(4 - (c * 0))) * (1 + 0)) -> (~4)
  return (mul i32 (
    bnot i32 (sub i32 (
      constval i32 0x4,
      mul i32 (dread i32 %c, constval i32 0x0))),
    add i32 (constval i32 0x1, constval i32 0x0)))
}


func &testConstFoldDiv0 (var %a i32, var %b i32, var %c i32) i32 {
  # (5 / 0) * (1 + 1) -> 2
  return (mul i32 (
    div i32 (constval i32 0x5, constval i32 0x0),
    add i32 (constval i32 0x1, constval i32 0x1)))
}

func &testConstFoldFloor00 (var %a i32, var %b i32, var %c i32) i32 {
  # (floor(4.25) + c) * (1 + 1) -> (c + 4) * 2
  return (mul i32 (
    add i32 (floor i32 f32 (constval f32 4.25f), dread i32 %c),
    add i32 (constval i32 0x1, constval i32 0x1)))
}

func &testConstFoldFloor01 (var %a i32, var %b i32, var %c i32) i32 {
  # (floor(4.25) + 2) * (1 + 1) -> 12
  return (mul i32 (
    add i32 (floor i32 f32 (constval f32 4.25f), constval i32 0x2),
    add i32 (constval i32 0x1, constval i32 0x1)))
}

func &testConstFoldFloor02 (var %a i32, var %b i32, var %c i32) i32 {
  # floor((floor(4.25) + 1) + (1 + a)) -> floor(a + 6)
  return (floor i32 i32 (add i32 (
    add i32 (floor i32 f32 (constval f32 4.25f), constval i32 0x1),
    add i32 (constval i32 0x1, dread i32 %a))))
}

func &testConstFoldExtractbitsNode00 () i32 {
  # 487 = 00111100111
  # extractbits i32 6 5 (487) = 7 = 0111
  return (extractbits i32 6 5 (constval i32 487))
}

func &testConstFoldExtractbitsNode01 () i32 {
  # 487 = 00111100111
  # extractbits i32 4 5 (487) = -2 = 0xfffffffffffffffe
  return (extractbits i32 4 5 (constval i32 487))
}

func &testConstFoldExtractbitsNode02 (var %a i32, var %b i32, var %c i32) i32 {
  # 487 = 00111100111
  # extractbits i32 4 5 (487) = -2 = 0xfffffffffffffffe
  # extractbits((extractbits(487) + 1) + (1 + a)) -> extractbits(a)
  return (extractbits i32 4 5 (add i32 (
    add i32 (extractbits i32 4 5 (constval i32 487), constval i32 0x1),
    add i32 (constval i32 0x1, dread i32 %a))))
}

func &testConstFoldExtractbitsNode03 (var %a i32, var %b i32, var %c i32) i32 {
  # 487 = 00111100111
  # extractbits i32 6 5 (487) = 7 = 0111
  # extractbits((extractbits(487) + 1) + (1 + a)) -> extractbits(a + 9)
  return (extractbits i32 6 5 (add i32 (
    add i32 (extractbits i32 6 5 (constval i32 487), constval i32 0x1),
    add i32 (constval i32 0x1, dread i32 %a))))
}

func &testConstFoldCompareNode00 (var %a i32, var %b i32, var %c i32) i32 {
  # (ge(6, 2) + 1) + (1 + 2) -> 5
  return (add i32 (
    add i32 (
      ge i32 i32 (constval i32 6, constval i32 2),
      constval i32 1),
    add i32 (constval i32 1, constval i32 2)))
}

func &testConstFoldCompareNode01 (var %a i32, var %b i32, var %c i32) i32 {
  # ge((ge(6, 2) + 1), (1 + a)) -> ge(2, a + 1)
  return (ge i32 i32 (
    add i32 (
      ge i32 i32 (constval i32 6, constval i32 2),
      constval i32 1),
    add i32 (constval i32 1, dread i32 %a)))
}

func &testConstFoldTernaryNode00 (var %a i32, var %b i32, var %c i32) i32 {
  # (6 > 2 ? 5 : 0) + (1 + 2) -> 8
  return ( add i32 (
    select i32 (
      ge i32 i32 (constval i32 6, constval i32 2),
      constval i32 5,
      constval i32 0),
    add i32 (constval i32 1,  constval i32 2)))
}

func &testConstFoldTernaryNode01 (var %a i32, var %b i32, var %c i32) i32 {
  # (6 > 10 ? 5 : 0) + (1 + a) -> (a + 1)
  return ( add i32 (
    select i32 (
      ge i32 i32 (constval i32 6, constval i32 10),
      constval i32 5,
      constval i32 0),
    add i32 (constval i32 1, dread i32 %a)))
}

func &testConstFoldTernaryNode02 (var %a i32, var %b i32, var %c i32) i32 {
  # 6 > a ? (1 + 6) + (1 + a) : 0 -> 6 > a ? a + 8 : 0
  return ( select i32 (
    ge i32 i32 (constval i32 6, dread i32 %a),
    add i32 (
      add i32 (constval i32 1, constval i32 6),
      add i32 (constval i32 1, dread i32 %a)),
    constval i32 0))
}

type $Hello <struct {
    @h i32,
    @e i32,
    @l i32,
    @o i32
  }>

type $Person <struct {
  @age i32,
  @extra <$Hello>,
  @sex u1}>


func &testConstFoldIassignNode00 (var %a i32) void {
  # this is equivalent to dassign %a 0 (constval i32 0x6)
  iassign <* i32> 0 (addrof ptr %a 0, add i32 (constval i32 1, constval i32 5))
}

func &testConstFoldIassignNode01 (var %a i32) void {
  var %bob $Person

  # this is equivalent to dassign %bob 5 (constval i32 0x6)
  iassign <* $Hello> 3 (addrof ptr %bob 2, add i32 (constval i32 1, constval i32 5))
}


func &testConstFoldIassignNode02 (var %a i32) void {
  # this is equivalent to dassign %a 0 (constval i32 0x6)
  iassign <* i32> 0 (
    iaddrof ptr <* i32> 0 (addrof ptr %a 0),
    add i32 (constval i32 1, constval i32 5))
}

func &testConstFoldIassignNode03 (var %a i32) void {

  var %bob $Person
  # this is equivalent to dassign %bob 5 (constval i32 0x6)
  iassign <* <$Hello>> 3 (
    iaddrof ptr <* <$Person>> 2 (addrof ptr %bob 0),
    add i32 (constval i32 1, constval i32 5))
}

func &testConstFoldIassignNode04 (var %a i32) void {

  var %bob $Person
  # this is equivalent to dassign %bob 5 (constval i32 0x6)
  iassign <* <$Hello>> 3 (
    iaddrof ptr <* <$Hello>> 0 (addrof ptr %bob 2),
    add i32 (constval i32 1, constval i32 5))
}


func &testConstFoldIassignNode05 (var %a i32) void {
  var %b <* i32>

  dassign %b (addrof ptr %a 0)

  # this is equivalent to iassign <* i32> 0 (dread ptr %a, constval i32 0x6)
  iassign <* i32> 0 (
    iaddrof ptr <* i32> 0 (dread ptr %a),
    add i32 (constval i32 1, constval i32 5))
}

func &testConstFoldIassignNode06 (var %a i32) void {
  var %bob $Person
  # this is equivalent to dassign %a 0 (dread i32 %bob 1)
  dassign %a 0 (iread agg <* <$Person>> 1 (addrof ptr %bob 0))
}

func &testConstFoldSwitchNode00 () void {
  # switch (1+4) ... -> goto @lab1
  switch ( add i32 (constval i32 1, constval i32 4)) @labdft {
  -2: goto @lab0
  5: goto @lab1
  8: goto @lab9 }

  @lab0
   return (constval i32 1)
  @labdft
   return (constval i32 2)
  @lab9
   return (constval i32 3)
  @lab1
   return (constval i32 4)
}

func &testConstFoldSwitchNode01 () void {
  # switch (1 + 10) ... -> goto @labdft
  switch ( add i32 (constval i32 1, constval i32 10)) @labdft {
  -2: goto @lab0
  5: goto @lab1
  8: goto @lab9 }

  @lab0
   return (constval i32 1)
  @labdft
   return (constval i32 2)
  @lab9
   return (constval i32 3)
  @lab1
   return (constval i32 4)
}

func &testConstFoldSwitchNode02 (var %a i32) void {
  # switch (5 + (5 - a)) ... -> switch (10 - a) ...
  switch (add i32 (
    constval i32 5,
    sub i32 (constval i32 5, dread i32 %a))) @labdft {
  -2: goto @lab0
  5: goto @lab1
  8: goto @lab9 }

  @lab0
   return (constval i32 1)
  @labdft
   return (constval i32 2)
  @lab9
   return (constval i32 3)
  @lab1
   return (constval i32 4)
}


func &testConstFoldArrayNode00 (var %x i32) void {
  var %a <* [15][15] i32>
  # fold indexes -> array ptr <* <[15][15] i32>> (constval i32 0x6, constval i32 0x4)
  dassign %x (
    iread i32 <* i32> (
      array 1 ptr <* [15][15] i32> (addrof ptr %a,
                                add i32 (constval i32 1, constval i32 5),
                                sub i32 (constval i32 5, constval i32 1))))
}

func &testDepositbitsNodeNode00 (var %a i32, var %c i32) i32 {
  return ( depositbits i32 1 23 (
             add i32 (
               add i32 (dread i32 %a, constval i32 0x5),
               add i32 (
                 add i32 (constval i32 0x2, dread i32 %c),
                 add i32 (constval i32 0x4, constval i32 0x2))),
             add i32 (constval i32 0x4, constval i32 0x8)))
}

func &testDepositbitsNodeNode01 () i32 {
  return ( depositbits i32 1 23 (
             add i32 (
               add i32 (constval i32 0x5, constval i32 0x5),
               add i32 (
                 add i32 (constval i32 0x2, constval i32 0x2),
                 add i32 (constval i32 0x4, constval i32 0x2))),
             add i32 (constval i32 0x4, constval i32 0x8)))
}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
