#if hc_PE
    #define jni_EXPORT hc_DLLEXPORT
#else
    #define jni_EXPORT
#endif

union jni_value {
    uint8_t z;
    int8_t b;
    uint16_t c;
    int16_t s;
    int32_t i;
    int64_t j;
    float f;
    double d;
    void *l;
};

struct jni_nativeMethod {
    char *name;
    char *signature;
    void *fnPtr;
};

enum jni_objectRefType {
     jni_INVALID_REF_TYPE = 0,
     jni_LOCAL_REF_TYPE = 1,
     jni_GLOBAL_REF_TYPE = 2,
     jni_WEAK_GLOBAL_REF_TYPE = 3
};

struct jni_invokeInterface {
    void *reserved[3];

    int32_t (*destroyJavaVM)(struct jni_invokeInterface **vm);
    int32_t (*attachCurrentThread)(struct jni_invokeInterface **vm, void **penv, void *args);
    int32_t (*detachCurrentThread)(struct jni_invokeInterface **vm);
    int32_t (*getEnv)(struct jni_invokeInterface **vm, void **penv, int32_t version);
    int32_t (*attachCurrentThreadAsDaemon)(struct jni_invokeInterface **vm, void **penv, void *args);
};

struct jni_env {
    void *reserved[4];

    int32_t (*getVersion)(struct jni_env **env);

    void *(*defineClass)(struct jni_env **env, const char *name, void *loader, const int8_t *buf, int32_t len);
    void *(*findClass)(struct jni_env **env, const char *name);

    void *(*fromReflectedMethod)(struct jni_env **env, void *method);
    void *(*fromReflectedField)(struct jni_env **env, void *field);

    void *(*toReflectedMethod)(struct jni_env **env, void *class, void *methodId, uint8_t isStatic);

    void *(*getSuperclass)(struct jni_env **env, void *sub);
    uint8_t (*isAssignableFrom)(struct jni_env **env, void *sub, void *sup);

    void *(*toReflectedField)(struct jni_env **env, void *class, void *fieldId, uint8_t isStatic);

    int32_t (*throw)(struct jni_env **env, void *obj);
    int32_t (*throwNew)(struct jni_env **env, void *class, const char *msg);
    void *(*exceptionOccurred)(struct jni_env **env);
    void (*exceptionDescribe)(struct jni_env **env);
    void (*exceptionClear)(struct jni_env **env);
    void (*fatalError)(struct jni_env **env, const char *msg);

    int32_t (*pushLocalFrame)(struct jni_env **env, int32_t capacity);
    void *(*popLocalFrame)(struct jni_env **env, void *result);

    void *(*newGlobalRef)(struct jni_env **env, void *lobj);
    void (*deleteGlobalRef)(struct jni_env **env, void *gref);
    void (*deleteLocalRef)(struct jni_env **env, void *obj);
    uint8_t (*isSameObject)(struct jni_env **env, void *obj1, void *obj2);
    void *(*newLocalRef)(struct jni_env **env, void *ref);
    int32_t (*ensureLocalCapacity)(struct jni_env **env, int32_t capacity);

    void *(*allocObject)(struct jni_env **env, void *class);
    void *(*newObject)(struct jni_env **env, void *class, void *methodId, ...);
    void *(*newObjectV)(struct jni_env **env, void *class, void *methodId, va_list args);
    void *(*newObjectA)(struct jni_env **env, void *class, void *methodId, const union jni_value *args);

    void *(*getObjectClass)(struct jni_env **env, void *obj);
    uint8_t (*isInstanceOf)(struct jni_env **env, void *obj, void *class);

    void *(*getMethodId)(struct jni_env **env, void *class, const char *name, const char *sig);

    void *(*callObjectMethod)(struct jni_env **env, void *obj, void *methodId, ...);
    void *(*callObjectMethodV)(struct jni_env **env, void *obj, void *methodId, va_list args);
    void *(*callObjectMethodA)(struct jni_env **env, void *obj, void *methodId, const union jni_value *args);

    uint8_t (*callBooleanMethod)(struct jni_env **env, void *obj, void *methodId, ...);
    uint8_t (*callBooleanMethodV)(struct jni_env **env, void *obj, void *methodId, va_list args);
    uint8_t (*callBooleanMethodA)(struct jni_env **env, void *obj, void *methodId, const union jni_value *args);

    int8_t (*callByteMethod)(struct jni_env **env, void *obj, void *methodId, ...);
    int8_t (*callByteMethodV)(struct jni_env **env, void *obj, void *methodId, va_list args);
    int8_t (*callByteMethodA)(struct jni_env **env, void *obj, void *methodId, const union jni_value *args);

    uint16_t (*callCharMethod)(struct jni_env **env, void *obj, void *methodId, ...);
    uint16_t (*callCharMethodV)(struct jni_env **env, void *obj, void *methodId, va_list args);
    uint16_t (*callCharMethodA)(struct jni_env **env, void *obj, void *methodId, const union jni_value *args);

    int16_t (*callShortMethod)(struct jni_env **env, void *obj, void *methodId, ...);
    int16_t (*callShortMethodV)(struct jni_env **env, void *obj, void *methodId, va_list args);
    int16_t (*callShortMethodA)(struct jni_env **env, void *obj, void *methodId, const union jni_value *args);

    int32_t (*callIntMethod)(struct jni_env **env, void *obj, void *methodId, ...);
    int32_t (*callIntMethodV)(struct jni_env **env, void *obj, void *methodId, va_list args);
    int32_t (*callIntMethodA)(struct jni_env **env, void *obj, void *methodId, const union jni_value *args);

    int64_t (*callLongMethod)(struct jni_env **env, void *obj, void *methodId, ...);
    int64_t (*callLongMethodV)(struct jni_env **env, void *obj, void *methodId, va_list args);
    int64_t (*callLongMethodA)(struct jni_env **env, void *obj, void *methodId, const union jni_value *args);

    float (*callFloatMethod)(struct jni_env **env, void *obj, void *methodId, ...);
    float (*callFloatMethodV)(struct jni_env **env, void *obj, void *methodId, va_list args);
    float (*callFloatMethodA)(struct jni_env **env, void *obj, void *methodId, const union jni_value *args);

    double (*callDoubleMethod)(struct jni_env **env, void *obj, void *methodId, ...);
    double (*callDoubleMethodV)(struct jni_env **env, void *obj, void *methodId, va_list args);
    double (*callDoubleMethodA)(struct jni_env **env, void *obj, void *methodId, const union jni_value *args);

    void (*callVoidMethod)(struct jni_env **env, void *obj, void *methodId, ...);
    void (*callVoidMethodV)(struct jni_env **env, void *obj, void *methodId, va_list args);
    void (*callVoidMethodA)(struct jni_env **env, void *obj, void *methodId, const union jni_value *args);

    void *(*callNonvirtualObjectMethod)(struct jni_env **env, void *obj, void *class, void *methodId, ...);
    void *(*callNonvirtualObjectMethodV)(struct jni_env **env, void *obj, void *class, void *methodId, va_list args);
    void *(*callNonvirtualObjectMethodA)(struct jni_env **env, void *obj, void *class, void *methodId, const union jni_value *args);

    uint8_t (*callNonvirtualBooleanMethod)(struct jni_env **env, void *obj, void *class, void *methodId, ...);
    uint8_t (*callNonvirtualBooleanMethodV)(struct jni_env **env, void *obj, void *class, void *methodId, va_list args);
    uint8_t (*callNonvirtualBooleanMethodA)(struct jni_env **env, void *obj, void *class, void *methodId, const union jni_value *args);

    int8_t (*callNonvirtualByteMethod)(struct jni_env **env, void *obj, void *class, void *methodId, ...);
    int8_t (*callNonvirtualByteMethodV)(struct jni_env **env, void *obj, void *class, void *methodId, va_list args);
    int8_t (*callNonvirtualByteMethodA)(struct jni_env **env, void *obj, void *class, void *methodId, const union jni_value *args);

    uint16_t (*callNonvirtualCharMethod)(struct jni_env **env, void *obj, void *class, void *methodId, ...);
    uint16_t (*callNonvirtualCharMethodV)(struct jni_env **env, void *obj, void *class, void *methodId, va_list args);
    uint16_t (*callNonvirtualCharMethodA)(struct jni_env **env, void *obj, void *class, void *methodId, const union jni_value *args);

    int16_t (*callNonvirtualShortMethod)(struct jni_env **env, void *obj, void *class, void *methodId, ...);
    int16_t (*callNonvirtualShortMethodV)(struct jni_env **env, void *obj, void *class, void *methodId, va_list args);
    int16_t (*callNonvirtualShortMethodA)(struct jni_env **env, void *obj, void *class, void *methodId, const union jni_value *args);

    int32_t (*callNonvirtualIntMethod)(struct jni_env **env, void *obj, void *class, void *methodId, ...);
    int32_t (*callNonvirtualIntMethodV)(struct jni_env **env, void *obj, void *class, void *methodId, va_list args);
    int32_t (*callNonvirtualIntMethodA)(struct jni_env **env, void *obj, void *class, void *methodId, const union jni_value *args);

    int64_t (*callNonvirtualLongMethod)(struct jni_env **env, void *obj, void *class, void *methodId, ...);
    int64_t (*callNonvirtualLongMethodV)(struct jni_env **env, void *obj, void *class, void *methodId, va_list args);
    int64_t (*callNonvirtualLongMethodA)(struct jni_env **env, void *obj, void *class, void *methodId, const union jni_value *args);

    float (*callNonvirtualFloatMethod)(struct jni_env **env, void *obj, void *class, void *methodId, ...);
    float (*callNonvirtualFloatMethodV)(struct jni_env **env, void *obj, void *class, void *methodId, va_list args);
    float (*callNonvirtualFloatMethodA)(struct jni_env **env, void *obj, void *class, void *methodId, const union jni_value *args);

    double (*callNonvirtualDoubleMethod)(struct jni_env **env, void *obj, void *class, void *methodId, ...);
    double (*callNonvirtualDoubleMethodV)(struct jni_env **env, void *obj, void *class, void *methodId, va_list args);
    double (*callNonvirtualDoubleMethodA)(struct jni_env **env, void *obj, void *class, void *methodId, const union jni_value *args);

    void (*callNonvirtualVoidMethod)(struct jni_env **env, void *obj, void *class, void *methodId, ...);
    void (*callNonvirtualVoidMethodV)(struct jni_env **env, void *obj, void *class, void *methodId, va_list args);
    void (*callNonvirtualVoidMethodA)(struct jni_env **env, void *obj, void *class, void *methodId, const union jni_value *args);

    void *(*getFieldId)(struct jni_env **env, void *class, const char *name, const char *sig);

    void *(*getObjectField)(struct jni_env **env, void *obj, void *fieldId);
    uint8_t (*getBooleanField)(struct jni_env **env, void *obj, void *fieldId);
    int8_t (*getByteField)(struct jni_env **env, void *obj, void *fieldId);
    uint16_t (*getCharField)(struct jni_env **env, void *obj, void *fieldId);
    int16_t (*getShortField)(struct jni_env **env, void *obj, void *fieldId);
    int32_t (*getIntField)(struct jni_env **env, void *obj, void *fieldId);
    int64_t (*getLongField)(struct jni_env **env, void *obj, void *fieldId);
    float (*getFloatField)(struct jni_env **env, void *obj, void *fieldId);
    double (*getDoubleField)(struct jni_env **env, void *obj, void *fieldId);

    void (*setObjectField)(struct jni_env **env, void *obj, void *fieldId, void *val);
    void (*setBooleanField)(struct jni_env **env, void *obj, void *fieldId, uint8_t val);
    void (*setByteField)(struct jni_env **env, void *obj, void *fieldId, int8_t val);
    void (*setCharField)(struct jni_env **env, void *obj, void *fieldId, uint16_t val);
    void (*setShortField)(struct jni_env **env, void *obj, void *fieldId, int16_t val);
    void (*setIntField)(struct jni_env **env, void *obj, void *fieldId, int32_t val);
    void (*setLongField)(struct jni_env **env, void *obj, void *fieldId, int64_t val);
    void (*setFloatField)(struct jni_env **env, void *obj, void *fieldId, float val);
    void (*setDoubleField)(struct jni_env **env, void *obj, void *fieldId, double val);

    void *(*getStaticMethodId)(struct jni_env **env, void *class, const char *name, const char *sig);

    void *(*callStaticObjectMethod)(struct jni_env **env, void *class, void *methodId, ...);
    void *(*callStaticObjectMethodV)(struct jni_env **env, void *class, void *methodId, va_list args);
    void *(*callStaticObjectMethodA)(struct jni_env **env, void *class, void *methodId, const union jni_value *args);

    uint8_t (*callStaticBooleanMethod)(struct jni_env **env, void *class, void *methodId, ...);
    uint8_t (*callStaticBooleanMethodV)(struct jni_env **env, void *class, void *methodId, va_list args);
    uint8_t (*callStaticBooleanMethodA)(struct jni_env **env, void *class, void *methodId, const union jni_value *args);

    int8_t (*callStaticByteMethod)(struct jni_env **env, void *class, void *methodId, ...);
    int8_t (*callStaticByteMethodV)(struct jni_env **env, void *class, void *methodId, va_list args);
    int8_t (*callStaticByteMethodA)(struct jni_env **env, void *class, void *methodId, const union jni_value *args);

    uint16_t (*callStaticCharMethod)(struct jni_env **env, void *class, void *methodId, ...);
    uint16_t (*callStaticCharMethodV)(struct jni_env **env, void *class, void *methodId, va_list args);
    uint16_t (*callStaticCharMethodA)(struct jni_env **env, void *class, void *methodId, const union jni_value *args);

    int16_t (*callStaticShortMethod)(struct jni_env **env, void *class, void *methodId, ...);
    int16_t (*callStaticShortMethodV)(struct jni_env **env, void *class, void *methodId, va_list args);
    int16_t (*callStaticShortMethodA)(struct jni_env **env, void *class, void *methodId, const union jni_value *args);

    int32_t (*callStaticIntMethod)(struct jni_env **env, void *class, void *methodId, ...);
    int32_t (*callStaticIntMethodV)(struct jni_env **env, void *class, void *methodId, va_list args);
    int32_t (*callStaticIntMethodA)(struct jni_env **env, void *class, void *methodId, const union jni_value *args);

    int64_t (*callStaticLongMethod)(struct jni_env **env, void *class, void *methodId, ...);
    int64_t (*callStaticLongMethodV)(struct jni_env **env, void *class, void *methodId, va_list args);
    int64_t (*callStaticLongMethodA)(struct jni_env **env, void *class, void *methodId, const union jni_value *args);

    float (*callStaticFloatMethod)(struct jni_env **env, void *class, void *methodId, ...);
    float (*callStaticFloatMethodV)(struct jni_env **env, void *class, void *methodId, va_list args);
    float (*callStaticFloatMethodA)(struct jni_env **env, void *class, void *methodId, const union jni_value *args);

    double (*callStaticDoubleMethod)(struct jni_env **env, void *class, void *methodId, ...);
    double (*callStaticDoubleMethodV)(struct jni_env **env, void *class, void *methodId, va_list args);
    double (*callStaticDoubleMethodA)(struct jni_env **env, void *class, void *methodId, const union jni_value *args);

    void (*callStaticVoidMethod)(struct jni_env **env, void *class, void *methodId, ...);
    void (*callStaticVoidMethodV)(struct jni_env **env, void *class, void *methodId, va_list args);
    void (*callStaticVoidMethodA)(struct jni_env **env, void *class, void *methodId, const union jni_value *args);

    void *(*getStaticFieldId)(struct jni_env **env, void *class, const char *name, const char *sig);
    void *(*getStaticObjectField)(struct jni_env **env, void *class, void *fieldId);
    uint8_t (*getStaticBooleanField)(struct jni_env **env, void *class, void *fieldId);
    int8_t (*getStaticByteField)(struct jni_env **env, void *class, void *fieldId);
    uint16_t (*getStaticCharField)(struct jni_env **env, void *class, void *fieldId);
    int16_t (*getStaticShortField)(struct jni_env **env, void *class, void *fieldId);
    int32_t (*getStaticIntField)(struct jni_env **env, void *class, void *fieldId);
    int64_t (*getStaticLongField)(struct jni_env **env, void *class, void *fieldId);
    float (*getStaticFloatField)(struct jni_env **env, void *class, void *fieldId);
    double (*getStaticDoubleField)(struct jni_env **env, void *class, void *fieldId);

    void (*setStaticObjectField)(struct jni_env **env, void *class, void *fieldId, void *value);
    void (*setStaticBooleanField)(struct jni_env **env, void *class, void *fieldId, uint8_t value);
    void (*setStaticByteField)(struct jni_env **env, void *class, void *fieldId, int8_t value);
    void (*setStaticCharField)(struct jni_env **env, void *class, void *fieldId, uint16_t value);
    void (*setStaticShortField)(struct jni_env **env, void *class, void *fieldId, int16_t value);
    void (*setStaticIntField)(struct jni_env **env, void *class, void *fieldId, int32_t value);
    void (*setStaticLongField)(struct jni_env **env, void *class, void *fieldId, int64_t value);
    void (*setStaticFloatField)(struct jni_env **env, void *class, void *fieldId, float value);
    void (*setStaticDoubleField)(struct jni_env **env, void *class, void *fieldId, double value);

    void *(*newString)(struct jni_env **env, const uint16_t *unicode, int32_t len);
    int32_t (*getStringLength)(struct jni_env **env, void *str);
    const uint16_t *(*getStringChars)(struct jni_env **env, void *str, uint8_t *isCopy);
    void (*releaseStringChars)(struct jni_env **env, void *str, const uint16_t *chars);

    void *(*newStringUTF)(struct jni_env **env, const char *utf);
    int32_t (*getStringUTFLength)(struct jni_env **env, void *str);
    const char* (*getStringUTFChars)(struct jni_env **env, void *str, uint8_t *isCopy);
    void (*releaseStringUTFChars)(struct jni_env **env, void *str, const char *chars);

    int32_t (*getArrayLength)(struct jni_env **env, void *array);

    void *(*newObjectArray)(struct jni_env **env, int32_t len, void *class, void *init);
    void *(*getObjectArrayElement)(struct jni_env **env, void *array, int32_t index);
    void (*setObjectArrayElement)(struct jni_env **env, void *array, int32_t index, void *val);

    void *(*newBooleanArray)(struct jni_env **env, int32_t len);
    void *(*newByteArray)(struct jni_env **env, int32_t len);
    void *(*newCharArray)(struct jni_env **env, int32_t len);
    void *(*newShortArray)(struct jni_env **env, int32_t len);
    void *(*newIntArray)(struct jni_env **env, int32_t len);
    void *(*newLongArray)(struct jni_env **env, int32_t len);
    void *(*newFloatArray)(struct jni_env **env, int32_t len);
    void *(*newDoubleArray)(struct jni_env **env, int32_t len);

    uint8_t *(*getBooleanArrayElements)(struct jni_env **env, void *array, uint8_t *isCopy);
    int8_t *(*getByteArrayElements)(struct jni_env **env, void *array, uint8_t *isCopy);
    uint16_t *(*getCharArrayElements)(struct jni_env **env, void *array, uint8_t *isCopy);
    int16_t *(*getShortArrayElements)(struct jni_env **env, void *array, uint8_t *isCopy);
    int32_t *(*getIntArrayElements)(struct jni_env **env, void *array, uint8_t *isCopy);
    int64_t *(*getLongArrayElements)(struct jni_env **env, void *array, uint8_t *isCopy);
    float *(*getFloatArrayElements)(struct jni_env **env, void *array, uint8_t *isCopy);
    double *(*getDoubleArrayElements)(struct jni_env **env, void *array, uint8_t *isCopy);

    void (*releaseBooleanArrayElements)(struct jni_env **env, void *array, uint8_t *elems, int32_t mode);
    void (*releaseByteArrayElements)(struct jni_env **env, void *array, int8_t *elems, int32_t mode);
    void (*releaseCharArrayElements)(struct jni_env **env, void *array, uint16_t *elems, int32_t mode);
    void (*releaseShortArrayElements)(struct jni_env **env, void *array, int16_t *elems, int32_t mode);
    void (*releaseIntArrayElements)(struct jni_env **env, void *array, int32_t *elems, int32_t mode);
    void (*releaseLongArrayElements)(struct jni_env **env, void *array, int64_t *elems, int32_t mode);
    void (*releaseFloatArrayElements)(struct jni_env **env, void *array, float *elems, int32_t mode);
    void (*releaseDoubleArrayElements)(struct jni_env **env, void *array, double *elems, int32_t mode);

    void (*getBooleanArrayRegion)(struct jni_env **env, void *array, int32_t start, int32_t l, uint8_t *buf);
    void (*getByteArrayRegion)(struct jni_env **env, void *array, int32_t start, int32_t len, int8_t *buf);
    void (*getCharArrayRegion)(struct jni_env **env, void *array, int32_t start, int32_t len, uint16_t *buf);
    void (*getShortArrayRegion)(struct jni_env **env, void *array, int32_t start, int32_t len, int16_t *buf);
    void (*getIntArrayRegion)(struct jni_env **env, void *array, int32_t start, int32_t len, int32_t *buf);
    void (*getLongArrayRegion)(struct jni_env **env, void *array, int32_t start, int32_t len, int64_t *buf);
    void (*getFloatArrayRegion)(struct jni_env **env, void *array, int32_t start, int32_t len, float *buf);
    void (*getDoubleArrayRegion)(struct jni_env **env, void *array, int32_t start, int32_t len, double *buf);

    void (*setBooleanArrayRegion)(struct jni_env **env, void *array, int32_t start, int32_t l, const uint8_t *buf);
    void (*setByteArrayRegion)(struct jni_env **env, void *array, int32_t start, int32_t len, const int8_t *buf);
    void (*setCharArrayRegion)(struct jni_env **env, void *array, int32_t start, int32_t len, const uint16_t *buf);
    void (*setShortArrayRegion)(struct jni_env **env, void *array, int32_t start, int32_t len, const int16_t *buf);
    void (*setIntArrayRegion)(struct jni_env **env, void *array, int32_t start, int32_t len, const int32_t *buf);
    void (*setLongArrayRegion)(struct jni_env **env, void *array, int32_t start, int32_t len, const int64_t *buf);
    void (*setFloatArrayRegion)(struct jni_env **env, void *array, int32_t start, int32_t len, const float *buf);
    void (*setDoubleArrayRegion)(struct jni_env **env, void *array, int32_t start, int32_t len, const double *buf);

    int32_t (*registerNatives)(struct jni_env **env, void *class, const struct jni_nativeMethod *methods, int32_t nMethods);
    int32_t (*unregisterNatives)(struct jni_env **env, void *class);

    int32_t (*monitorEnter)(struct jni_env **env, void *obj);
    int32_t (*monitorExit)(struct jni_env **env, void *obj);

    int32_t (*getJavaVM)(struct jni_env **env, struct jni_invokeInterface ***vm);

    void (*getStringRegion)(struct jni_env **env, void *str, int32_t start, int32_t len, uint16_t *buf);
    void (*getStringUTFRegion)(struct jni_env **env, void *str, int32_t start, int32_t len, char *buf);

    void *(*getPrimitiveArrayCritical)(struct jni_env **env, void *array, uint8_t *isCopy);
    void (*releasePrimitiveArrayCritical)(struct jni_env **env, void *array, void *carray, int32_t mode);

    const uint16_t * (*getStringCritical)(struct jni_env **env, void *string, uint8_t *isCopy);
    void (*releaseStringCritical)(struct jni_env **env, void *string, const uint16_t *cstring);

    void *(*newWeakGlobalRef)(struct jni_env **env, void *obj);
    void (*deleteWeakGlobalRef)(struct jni_env **env, void *ref);

    uint8_t (*exceptionCheck)(struct jni_env **env);

    void *(*newDirectByteBuffer)(struct jni_env* env, void* address, int64_t capacity);
    void *(*getDirectBufferAddress)(struct jni_env* env, void *buf);
    int64_t (*getDirectBufferCapacity)(struct jni_env* env, void *buf);

    enum jni_objectRefType (*getObjectRefType)(struct jni_env* env, void *obj);
    void *(*getModule)(struct jni_env* env, void *class);
    uint8_t (*isVirtualThread)(struct jni_env* env, void *obj);
};
