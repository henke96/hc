package jni;

public class Test {
    public static void main(String[] args) {
        System.loadLibrary("test");
        new Test().hello();
    }

    private native void hello();

    public void world() {
        System.out.println("world!");
    }
}
