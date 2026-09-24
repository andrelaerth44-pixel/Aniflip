package com.aniflip.engine

/**
 * Wrapper Kotlin sobre o motor nativo (C++) do Aniflip.
 *
 * Responsabilidades desta fase:
 *  - manter um ponteiro nativo (handle) para a instância do motor
 *  - expor operações de camada e frame
 *  - expor o traço de pincel (stroke) e o render composto (para exibir na UI)
 *
 * A lógica real (blending, buffers, stamps de pincel) vive em C++, em /engine.
 */
class AniflipEngine(width: Int, height: Int) {

    private var nativeHandle: Long = nativeCreate(width, height)

    fun newFrame() {
        check(nativeHandle != 0L) { "Engine já destruído" }
        nativeNewFrame(nativeHandle)
    }

    fun addLayer(name: String) {
        check(nativeHandle != 0L) { "Engine já destruído" }
        nativeAddLayer(nativeHandle, name)
    }

    fun strokeTo(x: Int, y: Int, brushSize: Int, argbColor: Int) {
        check(nativeHandle != 0L) { "Engine já destruído" }
        nativeStrokeTo(nativeHandle, x, y, brushSize, argbColor)
    }

    /** Retorna o composite atual (todas as camadas do frame corrente) como ARGB8888 IntArray. */
    fun renderComposite(): IntArray {
        check(nativeHandle != 0L) { "Engine já destruído" }
        return nativeRenderComposite(nativeHandle)
    }

    fun destroy() {
        if (nativeHandle != 0L) {
            nativeDestroy(nativeHandle)
            nativeHandle = 0L
        }
    }

    private external fun nativeCreate(width: Int, height: Int): Long
    private external fun nativeDestroy(handle: Long)
    private external fun nativeNewFrame(handle: Long)
    private external fun nativeAddLayer(handle: Long, name: String)
    private external fun nativeStrokeTo(handle: Long, x: Int, y: Int, brushSize: Int, argbColor: Int)
    private external fun nativeRenderComposite(handle: Long): IntArray

    companion object {
        init {
            System.loadLibrary("aniflip_engine")
        }
    }
}
