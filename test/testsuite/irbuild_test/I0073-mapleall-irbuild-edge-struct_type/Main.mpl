#struct aa{
#  double b;
#  float *x;
#};

#struct bb {
#  struct aa b1;
#  struct aa *pa;
#  double x1[10];
#  double dx;
#};

#typedef struct ss {
#  int f1;
#  char f2:3;
#  struct aa a3;
#  struct aa *pa1;
#  char f3:5;
#  struct bb *b3;
#  struct bb *b2;
#  struct aa xt[23];
#  struct aa(* funcc)(int, int, double);
#}SS;

type $unname1 <struct {@b f64,
                       @x <* f32>}>
type $unname2 <struct {@b1 <struct {@b f64, 
                                    @x <* f32>}>,
                       @pa <* <struct {@b f64, 
                                       @x <* f32>}>>, 
                       @x1 <[10] f64>,
                       @dx f64}>
type $SS      <struct {@f1 i32,
                       @f2:3 i8,
                       @a3 <structincomplete {@b f64, 
                                    @x <*f32>}>,
                       @pa1 <* <struct {@b f64, 
                                       @x <* f32> } >>,
                       @f3:5 i32,
                       @b3 <* <struct {@b1 <struct {@b f64,
                                                    @x <* f32>}>,
                                       @pa <* <struct {@b f64,
                                                       @x <* f32>}>>,
                                       @x1 <[10] f64>,
                                       @dx f64}>>,
                       @b2 <* $unname2>,
                       @xt <[23] <struct {@b f64, 
                                          @a <*f32>}>>,
                       @funcc <func(i32, i32, f64)<struct {@b f64,
                                                           @x <* f32>}>>}>

 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
