// Ponte JNI: expõe o motor nativo (FrameSequence + BrushEngine + LayerCompositor)
// para a classe Kotlin com.aniflip.engine.AniflipEngine.
//
// Convenção: um "handle" é o ponteiro para uma EngineSession, passado como Long
// entre Kotlin e C++ para manter o estado nativo vivo entre chamadas.
//
// nativeStrokeTo retorna um "patch" em vez do canvas inteiro: os 4 primeiros
// inteiros são [x0, y0, largura, altura] da região alterada, seguidos pelos
// pixels ARGB8888 dessa região. Isso evita recompor e transferir o canvas
// inteiro a cada movimento do dedo, que é o que causava o travamento.

#include <jni.h>

#include <memory>
#include <vector>

#include "aniflip/BrushEngine.h"
#include "aniflip/FrameSequence.h"
#include "aniflip/LayerCompositor.h"

namespace {

struct EngineSession {
    int width;
    int height;
    aniflip::FrameSequence sequence;
    aniflip::BrushEngine brush;

    EngineSession(int w, int h) : width(w), height(h) {}
};

inline EngineSession* toSession(jlong handle) {
    return reinterpret_cast<EngineSession*>(handle);
}

} // namespace

extern "C" {

JNIEXPORT jlong JNICALL
Java_com_aniflip_engine_AniflipEngine_nativeCreate(JNIEnv*, jobject, jint width, jint height) {
    auto* session = new EngineSession(width, height);
    return reinterpret_cast<jlong>(session);
}

JNIEXPORT void JNICALL
Java_com_aniflip_engine_AniflipEngine_nativeDestroy(JNIEnv*, jobject, jlong handle) {
    delete toSession(handle);
}

JNIEXPORT void JNICALL
Java_com_aniflip_engine_AniflipEngine_nativeNewFrame(JNIEnv*, jobject, jlong handle) {
    EngineSession* session = toSession(handle);
    session->sequence.addFrame();
    session->brush.beginStroke();
}

JNIEXPORT void JNICALL
Java_com_aniflip_engine_AniflipEngine_nativeAddLayer(JNIEnv* env, jobject, jlong handle, jstring name) {
    EngineSession* session = toSession(handle);
    const char* nameChars = env->GetStringUTFChars(name, nullptr);
    session->sequence.current().layers().addLayer(session->width, session->height, nameChars);
    env->ReleaseStringUTFChars(name, nameChars);
}

JNIEXPORT void JNICALL
Java_com_aniflip_engine_AniflipEngine_nativeBeginStroke(JNIEnv*, jobject, jlong handle) {
    EngineSession* session = toSession(handle);
    session->brush.beginStroke();
}

JNIEXPORT jintArray JNICALL
Java_com_aniflip_engine_AniflipEngine_nativeStrokeTo(
    JNIEnv* env, jobject, jlong handle, jint x, jint y, jint brushSize, jint argbColor) {
    EngineSession* session = toSession(handle);
    aniflip::LayerStack& stack = session->sequence.current().layers();
    if (stack.count() == 0) {
        stack.addLayer(session->width, session->height, "Camada 1");
    }
    aniflip::Layer& topLayer = stack.at(stack.count() - 1);
    aniflip::DirtyRect rect = session->brush.strokeTo(topLayer, x, y, brushSize, static_cast<uint32_t>(argbColor));

    if (rect.empty()) {
        jintArray result = env->NewIntArray(4);
        jint header[4] = {0, 0, 0, 0};
        env->SetIntArrayRegion(result, 0, 4, header);
        return result;
    }

    std::vector<uint32_t> patch = aniflip::LayerCompositor::compositeRegion(
        stack, session->width, session->height, rect.x0, rect.y0, rect.x1, rect.y1);

    const int patchWidth = rect.x1 - rect.x0 + 1;
    const int patchHeight = rect.y1 - rect.y0 + 1;

    jintArray result = env->NewIntArray(static_cast<jsize>(4 + patch.size()));
    jint header[4] = {rect.x0, rect.y0, patchWidth, patchHeight};
    env->SetIntArrayRegion(result, 0, 4, header);
    env->SetIntArrayRegion(
        result, 4, static_cast<jsize>(patch.size()),
        reinterpret_cast<const jint*>(patch.data()));
    return result;
}

JNIEXPORT jintArray JNICALL
Java_com_aniflip_engine_AniflipEngine_nativeRenderComposite(JNIEnv* env, jobject, jlong handle) {
    EngineSession* session = toSession(handle);
    std::vector<uint32_t> composite = aniflip::LayerCompositor::composite(
        session->sequence.current().layers(), session->width, session->height);

    jintArray result = env->NewIntArray(static_cast<jsize>(composite.size()));
    env->SetIntArrayRegion(
        result, 0, static_cast<jsize>(composite.size()),
        reinterpret_cast<const jint*>(composite.data()));
    return result;
}

} // extern "C"
