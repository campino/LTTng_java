/*
 * File:   jvm_agent.h
 * Author: Alexander Kampmann
 *
 * Created on January 4, 2013, 11:44 PM
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER java_ust

#ifdef	__cplusplus
extern "C" {
#endif

	#ifndef JVM_AGENT_H
	#define	JVM_AGENT_H

	#include <jni.h>
	#include <jvmti.h>

	#include <lttng/tracepoint.h>

	TRACEPOINT_EVENT(java_ust, method_entry, TP_ARGS(char *, clazz, char *, method), TP_FIELDS(ctf_string(method, name)))
	TRACEPOINT_EVENT(java_ust, method_exit, TP_ARGS(char *, clazz, char *, method), TP_FIELDS(ctf_string(method, name)))

	TRACEPOINT_LOGLEVEL(java_ust, method_entry, TRACE_INFO)
	TRACEPOINT_LOGLEVEL(java_ust, method_exit, TRACE_INFO)

	jvmtiEnv *jvmti;

	JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *vm, char *options, void *reserved);
	JNIEXPORT void JNICALL Agent_OnUnload(JavaVM *vm);

	void JNICALL cbMethodEntry(jvmtiEnv *jvmti_env,
            JNIEnv* jni_env,
            jthread thread,
            jmethodID method);

	void JNICALL cbMethodExit(jvmtiEnv *jvmti_env,
            JNIEnv* jni_env,
            jthread thread,
            jmethodID method,
            jboolean was_popped_by_exception,
            jvalue return_value);

#endif	/* JVM_AGENT_H */

#undef TRACEPOINT_INCLUDE_FILE
#define TRACEPOINT_INCLUDE_FILE ./jvm_agent.h

#include <lttng/tracepoint-event.h>

#ifdef	__cplusplus
}
#endif
