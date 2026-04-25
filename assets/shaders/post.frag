// post.frag — full-screen composite shader applied once per frame.
//
// Effects:
//   Chromatic aberration  — R/B channels split horizontally, driven by uTrauma.
//                           Zero at rest; peaks on damage for a hit-flash read.
//   Vignette              — radial darkening toward screen edges (replaces the
//                           CPU quad that used to live in Room.cpp).
//   Film grain            — low-amplitude animated noise for atmosphere.
//
// GLSL 1.10 — compatible with SFML 2.6 on OpenGL 2.0 targets.

uniform sampler2D texture;   // bound via sf::Shader::CurrentTexture
uniform float     uTime;     // elapsed seconds — animates grain pattern
uniform float     uTrauma;   // 0-1 from GameScene shake system

void main()
{
    vec2 uv = gl_TexCoord[0].xy;

    // --- Chromatic aberration -------------------------------------------------
    // Separate the red and blue channels by a small horizontal offset that
    // scales with trauma. At uTrauma=1 the offset is ~7 px at 1280 width.
    float aberr = uTrauma * 0.006;
    float r = texture2D(texture, uv + vec2( aberr, 0.0)).r;
    float g = texture2D(texture, uv).g;
    float b = texture2D(texture, uv - vec2( aberr, 0.0)).b;
    vec3 col = vec3(r, g, b);

    // --- Vignette ------------------------------------------------------------
    // Smooth circular darkening from center outward. smoothstep starts at
    // radius 0.45 (near the edge of a 0.5-radius circle) so the playfield
    // centre stays fully lit; corners darken to ~55% brightness.
    vec2  c    = uv - vec2(0.5, 0.5);
    float vign = 1.0 - smoothstep(0.45, 0.95, length(c));
    col *= mix(0.55, 1.0, vign);

    // --- Film grain ----------------------------------------------------------
    // Hash-based noise: deterministic per pixel per frame, zero net brightness.
    float seed  = dot(uv + fract(uTime * 0.07), vec2(127.1, 311.7));
    float grain = fract(sin(seed) * 43758.5453) - 0.5;
    col += grain * 0.025;

    gl_FragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
}
