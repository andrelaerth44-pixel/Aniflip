package com.aniflip.engine

/**
 * Wrapper Kotlin sobre o motor nativo (C++) do Aniflip.
 *
 * strokeTo() não retorna mais o canvas inteiro: retorna apenas o "patch" (região
 * que mudou), para a UI atualizar só aquele pedaço da tela em vez de recompor
 * tudo a cada movimento do dedo.
 */
class AniflipEngine(width: Int, height: Int) {

    /** Região retangular do canvas que mudou após um strokeTo(), com seus pixels ARGB8888. */
    data class DirtyPatch(
        val x: Int,
        val y: Int,
        val width: Int,
        val height: Int,
        val pixels: IntArray,
    )

    private var nativeHandle: Long = nativeCreate(width, height)

    fun newFrame() {
        check(nativeHandle != 0L) { "Engine já destruído" }
        nativeNewFrame(nativeHandle)
    }

    fun addLayer(name: String) {
        check(nativeHandle != 0L) { "Engine já destruído" }
        nativeAddLayer(nativeHandle, name)
    }

    /** Chame ao iniciar um novo traço (dedo tocou a tela), para não conectar com o traço anterior. */
    fun beginStroke() {
        check(nativeHandle != 0L) { "Engine já destruído" }
        nativeBeginStroke(nativeHandle)
    }

    /** Desenha até (x, y) e retorna apenas a região que mudou, ou null se nada mudou. */
    fun strokeTo(x: Int, y: Int, brushSize: Int, argbColor: Int): DirtyPatch? {
        check(nativeHandle != 0L) { "Engine já destruído" }
        val raw = nativeStrokeTo(nativeHandle, x, y, brushSize, argbColor)
        val patchWidth = raw[2]
        val patchHeight = raw[3]
        if (patchWidth <= 0 || patchHeight <= 0) return null
        val pixels = raw.copyOfRange(4, 4 + patchWidth * patchHeight)
        return DirtyPatch(x = raw[0], y = raw[1], width = patchWidth, height = patchHeight, pixels = pixels)
    }

    /** Retorna o composite completo (todas as camadas do frame corrente) como ARGB8888 IntArray. */
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
    private external fun nativeBeginStroke(handle: Long)
    private external fun nativeStrokeTo(handle: Long, x: Int, y: Int, brushSize: Int, argbColor: Int): IntArray
    private external fun nativeRenderComposite(handle: Long): IntArray

    companion object {
        init {
            System.loadLibrary("aniflip_engine")
        }
    }
}
