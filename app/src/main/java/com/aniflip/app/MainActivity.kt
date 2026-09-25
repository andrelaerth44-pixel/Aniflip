package com.aniflip.app

import android.graphics.Bitmap
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.layout.onSizeChanged
import androidx.compose.ui.unit.IntSize
import androidx.compose.ui.unit.dp
import com.aniflip.engine.AniflipEngine

/**
 * Fase 1: prova de que o motor nativo (Layer + LayerCompositor + BrushEngine)
 * funciona de ponta a ponta a partir do Kotlin, via JNI, com desenho por toque.
 *
 * As telas de Projetos, Timeline etc. virão nas próximas fases.
 */
class MainActivity : ComponentActivity() {

    private lateinit var engine: AniflipEngine

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val width = 800
        val height = 1200
        engine = AniflipEngine(width = width, height = height)
        engine.newFrame()
        engine.addLayer("Camada 1")

        setContent {
            MaterialTheme {
                Surface(modifier = Modifier.fillMaxSize()) {
                    EngineCanvasScreen(engine = engine, width = width, height = height)
                }
            }
        }
    }

    override fun onDestroy() {
        engine.destroy()
        super.onDestroy()
    }
}

@Composable
fun EngineCanvasScreen(engine: AniflipEngine, width: Int, height: Int) {
    // Bitmap mutável e persistente: nunca recriado a cada traço. Só seus pixels
    // são atualizados (na região suja), o que é ordens de magnitude mais barato
    // do que recompor e realocar o canvas inteiro a cada movimento do dedo.
    val bitmap = remember {
        Bitmap.createBitmap(engine.renderComposite(), width, height, Bitmap.Config.ARGB_8888)
            .copy(Bitmap.Config.ARGB_8888, true)
    }

    // Contador simples só para avisar o Compose "o bitmap mudou, redesenhe" sem
    // precisar recriar o objeto Bitmap inteiro a cada toque.
    var redrawTick by remember { mutableIntStateOf(0) }

    // Tamanho real (em pixels) que a Image está ocupando na tela. O bitmap do
    // motor tem resolução fixa (width x height) e o Compose estica essa imagem
    // para caber na tela — sem essa conversão de escala, a posição do toque não
    // bate com a posição do traço (era a causa da "distância enorme" reportada).
    var displayedSizePx by remember { mutableStateOf(IntSize(width, height)) }

    val brushSize by remember { mutableIntStateOf(18) }
    val brushColor = 0xFF1E1E1E.toInt()

    fun applyPatch(patch: AniflipEngine.DirtyPatch?) {
        if (patch == null) return
        bitmap.setPixels(patch.pixels, 0, patch.width, patch.x, patch.y, patch.width, patch.height)
        redrawTick++
    }

    fun toCanvasX(displayX: Float): Int {
        val scaleX = width.toFloat() / displayedSizePx.width.coerceAtLeast(1).toFloat()
        return (displayX * scaleX).toInt().coerceIn(0, width - 1)
    }

    fun toCanvasY(displayY: Float): Int {
        val scaleY = height.toFloat() / displayedSizePx.height.coerceAtLeast(1).toFloat()
        return (displayY * scaleY).toInt().coerceIn(0, height - 1)
    }

    Column(modifier = Modifier.fillMaxSize().padding(8.dp)) {
        Text("Aniflip — motor de desenho (fase 1)")

        // Ler redrawTick aqui garante que este bloco recomponha (e redesenhe a
        // Image com os pixels atualizados) sempre que um patch for aplicado.
        val tick = redrawTick

        Image(
            bitmap = bitmap.asImageBitmap(),
            contentDescription = "Canvas",
            modifier = Modifier
                .fillMaxSize()
                .background(Color.White)
                .onSizeChanged { size -> displayedSizePx = size }
                .pointerInput(Unit) {
                    detectDragGestures(
                        onDragStart = { offset ->
                            // Novo traço: reseta o motor para não conectar com o traço anterior.
                            engine.beginStroke()
                            val cx = toCanvasX(offset.x)
                            val cy = toCanvasY(offset.y)
                            applyPatch(engine.strokeTo(cx, cy, brushSize, brushColor))
                        },
                        onDrag = { change, _ ->
                            val cx = toCanvasX(change.position.x)
                            val cy = toCanvasY(change.position.y)
                            applyPatch(engine.strokeTo(cx, cy, brushSize, brushColor))
                        },
                    )
                },
        )
    }
}
