/*
 * File:   jvm_agent.h
 * Author: Alexander Kampmann
 *
 * Created on January 4, 2013, 11:44 PM
 */

#ifndef JVM_AGENT_H
#define	JVM_AGENT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <jni.h>
#include <jvmti.h>

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

#ifdef	__cplusplus
}
#endif

#endif	/* JVM_AGENT_H */
