hc_EXPORT void Java_jni_Test_hello(struct jni_env **env, void *obj) {
    debug_print("Hello ");

    void *class = (*env)->getObjectClass(env, obj);
    void *methodId = (*env)->getMethodId(env, class, "world", "()V");
    (*env)->callVoidMethod(env, obj, methodId);
}
