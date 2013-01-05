#include <stdlib.h>
#include "jvm_agent.h"

#define TRACEPOINT_DEFINE
#define TRACEPOINT_CREATE_PROBES
#include "tracepoints.h"

inline void CHECK_JVMTI_ERROR(jvmtiError error, const char *message) {
	switch(error) {
		case JVMTI_ERROR_NONE: return;
		default: {
			fprintf(stderr, "jvmti error: %d\n", error);
			abort();
		}
	}
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *vm, char *options, void *reserved) {
	jvmtiEnv *jvmti;

	// get jvmti
	jint res = (*vm)->GetEnv(vm, (void **)&jvmti, JVMTI_VERSION_1_0);
    if (res != JNI_OK) {
		fprintf(stderr, "ERROR: Unable to access jvmti.");
		return -1;
	}

	// ask for the capabilities
	jvmtiCapabilities      capabilities;
	(void)memset(&capabilities,0, sizeof(capabilities));
	capabilities.can_generate_method_entry_events=1;
	capabilities.can_generate_method_exit_events=1;
	jvmtiError error = (*jvmti)->AddCapabilities(jvmti, &capabilities);
    CHECK_JVMTI_ERROR(error, "Unable to get necessary JVMTI capabilities.");

	// add the callbacks
	jvmtiEventCallbacks    callbacks;
	(void)memset(&callbacks,0, sizeof(callbacks));
	callbacks.MethodEntry = &cbMethodEntry;
	callbacks.MethodExit = &cbMethodExit;

	error = (*jvmti)->SetEventCallbacks(jvmti, &callbacks, (jint)sizeof(callbacks));
	CHECK_JVMTI_ERROR(error, "Could not register callbacks.");

	// set event notifications
	error = (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE,
			  JVMTI_EVENT_METHOD_ENTRY, (jthread)NULL);
	CHECK_JVMTI_ERROR(error, "Could not register entry callback");
	error = (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE,
			  JVMTI_EVENT_METHOD_EXIT, (jthread)NULL);
	CHECK_JVMTI_ERROR(error, "Could not register exit callback");

	return 0;
}

JNIEXPORT void JNICALL Agent_OnUnload(JavaVM *vm) {
	// nothing, program is about to die anyways
}

void JNICALL cbMethodEntry(jvmtiEnv *jvmti_env,
            JNIEnv* jni_env,
            jthread thread,
            jmethodID method) {
	jclass declaring;
	jvmtiError error = (*jvmti_env)->GetMethodDeclaringClass(jvmti_env, method, &declaring);
	CHECK_JVMTI_ERROR(error, "Could not get declaring class.");

	unsigned char *decName;
	error= (*jvmti_env)->GetClassSignature(jvmti_env, declaring, &decName, NULL);
	CHECK_JVMTI_ERROR(error, "Could not get class signature.");

	unsigned char *name;
	error = (*jvmti_env)->GetMethodName(jvmti_env, method, &name, NULL, NULL);

	tracepoint(java_ust, method_entry, decName, name);
	(*jvmti_env)->Deallocate(jvmti_env, name);
	(*jvmti_env)->Deallocate(jvmti_env, decName);
}

void JNICALL cbMethodExit(jvmtiEnv *jvmti_env,
		JNIEnv* jni_env,
		jthread thread,
		jmethodID method,
		jboolean was_popped_by_exception,
		jvalue return_value) {
	jclass declaring;
	jvmtiError error = (*jvmti_env)->GetMethodDeclaringClass(jvmti_env, method, &declaring);
	CHECK_JVMTI_ERROR(error, "Could not get declaring class.");

	unsigned char *decName;
	error= (*jvmti_env)->GetClassSignature(jvmti_env, declaring, &decName, NULL);
	CHECK_JVMTI_ERROR(error, "Could not get class signature.");

	unsigned char *name;
	error = (*jvmti_env)->GetMethodName(jvmti_env, method, &name, NULL, NULL);
	CHECK_JVMTI_ERROR(error, "Could not get method name.");

	tracepoint(java_ust, method_exit, decName, name);
	(*jvmti_env)->Deallocate(jvmti_env, name);
	(*jvmti_env)->Deallocate(jvmti_env, decName);
}