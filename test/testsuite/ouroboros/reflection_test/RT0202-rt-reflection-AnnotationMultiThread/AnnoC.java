import java.lang.annotation.*;

@Target(ElementType.TYPE)
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
public @interface AnnoC {
    int[] intA() default Integer.MAX_VALUE;

    double[] doubleA() default Double.MIN_VALUE;

    String[] stringA() default "";

    AnnoB[] annoBA() default @AnnoB;

    ENUMA[] enumA() default ENUMA.A;
}
