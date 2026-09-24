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
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.input.pointer.pointerInput
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
    var bitmap by remember {
        mutableStateOf(Bitmap.createBitmap(engine.renderComposite(), width, height, Bitmap.Config.ARGB_8888))
    }
    val brushSize by remember { mutableIntStateOf(18) }

    Column(modifier = Modifier.fillMaxSize().padding(8.dp)) {
        Text("Aniflip — motor de desenho (fase 1)")
        Image(
            bitmap = bitmap.asImageBitmap(),
            contentDescription = "Canvas",
            modifier = Modifier
                .fillMaxSize()
                .background(androidx.compose.ui.graphics.Color.White)
                .pointerInput(Unit) {
                    detectDragGestures { change, _ ->
                        val x = change.position.x.toInt()
                        val y = change.position.y.toInt()
                        engine.strokeTo(x, y, brushSize, 0xFF1E1E1E.toInt())
                        bitmap = Bitmap.createBitmap(engine.renderComposite(), width, height, Bitmap.Config.ARGB_8888)
                    }
                }
        )
    }
}
