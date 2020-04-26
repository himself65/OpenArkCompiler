import java.lang.annotation.*;

@Target(ElementType.TYPE_USE)
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
public @interface AnnoA {
    int intA();
    byte byteA();
    char charA();
    double doubleA();
    boolean booleanA();
    long longA();
    float floatA();
    short shortA();
    int[] intAA();
    byte[] byteAA();
    char[] charAA();
    double[] doubleAA();
    boolean[] booleanAA();
    long[] longAA();
    float[] floatAA();
    short[] shortAA();
    String stringA();
    String[] stringAA();
    Class classA();
    Class[] classAA();
    Thread.State stateA();
    Thread.State[] stateAA();
    AnnoB annoBA();
    AnnoB[] annoBAA();
}
