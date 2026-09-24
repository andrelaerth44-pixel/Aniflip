# Aniflip

App de animação 2D quadro a quadro (estilo FlipaClip), com motor de desenho em C++ e app Android em Kotlin/Jetpack Compose.

## Status do projeto

**Fase atual: motor de desenho (core engine).**

- [x] Estrutura do repositório
- [x] Motor nativo: `Layer`, `LayerCompositor`, `BrushEngine`, `Frame`/`FrameSequence`
- [x] Ponte JNI (Kotlin ↔ C++)
- [x] App Kotlin mínimo: tela de canvas desenhando via engine nativo
- [x] CI (GitHub Actions) compilando o APK de debug a cada push
- [ ] Tela de Projetos (criar/abrir/duplicar/excluir, escolher FPS e resolução)
- [ ] Timeline (frames + camadas, onion skin, duplicar/apagar frame)
- [ ] Ferramentas de pincel avançadas (pressão, textura, formas)
- [ ] Áudio (importar trilha, sincronizar com frames)
- [ ] Exportação (GIF, MP4, PNG sequence via FFmpeg)
- [ ] Recursos de IA (interpolação de frames, etc.)

## Arquitetura

```
app/                  -> módulo Android (Kotlin/Compose), UI
engine/               -> motor nativo em C++ (CMake), compilado via NDK
  include/aniflip/    -> headers públicos do motor
  src/                -> implementação
  jni/                -> ponte JNI exposta ao Kotlin
.github/workflows/    -> pipeline de build (GitHub Actions)
```

O motor nativo trabalha com buffers de pixels ARGB8888 puros nesta fase inicial (sem dependência externa de rasterização), para manter o primeiro build simples e 100% reproduzível em CI. Integração com uma biblioteca de rasterização mais avançada (ex.: Skia) fica para uma fase posterior, quando o pipeline de build já estiver validado ponta a ponta.

## Como compilar

Você não precisa de Android SDK/NDK instalado localmente: a cada push na branch `main`, o GitHub Actions compila o APK de debug automaticamente. Veja a aba **Actions** do repositório; o APK gerado fica disponível como artifact do workflow `android-ci`.

Para compilar localmente (opcional, se um dia tiver Android Studio): abra a pasta raiz no Android Studio com NDK e CMake instalados via SDK Manager, e rode o app normalmente.
