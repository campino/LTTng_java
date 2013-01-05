#include <stdlib.h>
#include "jvm_agent.h"

#define TRACEPOINT_DEFINE
#define TRACEPOINT_CREATE_PROBES
#include "tracepoints.h"

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *vm, char *options, void *reserved) {
	// get jvmti
	jint res = (*vm)->GetEnv(vm, (void **)&jvmti, JVMTI_VERSION_1_0);
    if (res != JNI_OK) {
		fatal_error("ERROR: Unable to access jvmti");
	}

	// ask for the capabilities
	jvmtiCapabilities      capabilities;
	(void)memset(&capabilities,0, sizeof(capabilities));
	capabilities.can_generate_method_entry_events=1;
	capabilities.can_generate_method_exit_events=1;
	jvmtiError error = (*jvmti)->AddCapabilities(jvmti, &capabilities);
    CHECK_JVMTI_ERROR(jvmti, error, "Unable to get necessary JVMTI capabilities.");

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
}

JNIEXPORT void JNICALL Agent_OnUnload(JavaVM *vm) {
	// nothing, program is about to die anyways
}

void JNICALL cbMethodEntry(jvmtiEnv *jvmti_env,
            JNIEnv* jni_env,
            jthread thread,
            jmethodID method) {
	jclass declaring;
	jvmtiError error = GetMethodDeclaringClass(jvmti_env, method, &declaring);
	CHECK_JVMTI_ERROR(error, "Could not get declaring class.");

	char *decName;
	error= GetClassSignature(jvmti_env, declaring, &decName, NULL);
	CHECK_JVMTI_ERROR(error, "Could not get class signature.");

	char *name;
	error = GetMethodName(jvmti_env, method, &name, NULL, NULL);

	tracepoint(java_ust, method_entry, decName, name);
	Deallocate(name);
	Deallocate(decName);
}

void JNICALL cbMethodExit(jvmtiEnv *jvmti_env,
		JNIEnv* jni_env,
		jthread thread,
		jmethodID method,
		jboolean was_popped_by_exception,
		jvalue return_value) {
	jclass declaring;
	jvmtiError error = GetMethodDeclaringClass(jvmti_env, method, &declaring);
	CHECK_JVMTI_ERROR(error, "Could not get declaring class.");

	char *decName;
	error= GetClassSignature(jvmti_env, declaring, &decName, NULL);
	CHECK_JVMTI_ERROR(error, "Could not get class signature.");

	char *name;
	error = GetMethodName(jvmti_env, method, &name, NULL, NULL);

	tracepoint(java_ust, method_exit, decName, name);
	Deallocate(name);
	Deallocate(decName);
}